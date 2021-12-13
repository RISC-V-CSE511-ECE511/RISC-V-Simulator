#include <fmt/core.h>

#include <algorithm>
#include <assembler/immediate.hpp>
#include <assembler/label.hpp>
#include <assembler/operand.hpp>
#include <assembler/register.hpp>
#include <assembler/register_displacement.hpp>
#include <cstdint>
#include <map>
#include <memory>
#include <range/v3/all.hpp>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

namespace assembler {
struct Instruction {
 private:
  std::string_view m_label_name;
  std::string m_raw_instr;
  std::string m_instr_name;
  std::string m_opcode;
  std::unordered_map<std::string, std::string> m_opcode_map{
      /*R Type*/
      { "add", "0000000{}{}000{}0110011" },
      { "sub", "0100000{}{}000{}0110011" },
      { "xor", "0000000{}{}100{}0110011" },
      { "and", "0000000{}{}111{}0110011" },
      { "or", "0000000{}{}110{}0110011" },
      { "sll", "0000000{}{}001{}0110011" },
      { "sra", "0100000{}{}101{}0110011" },

      /*I Type*/
      { "addi", "{}{}000{}0010011" },
      { "lw", "{}{}010{}0000011" },

      /*S*/
      { "sw", "{}{}{}010{}0100011" },

      /*UJ*/
      { "jal", "{}{}{}1101111" },
      { "jalr", "{}{}000{}1100111" },

      /*SB*/
      { "blt", "{}{}{}100{}1100011" },
      { "bge", "{}{}{}101{}1100011" },
      { "beq", "{}{}{}000{}1100011" },
      { "bne", "{}{}{}001{}1100011" },

      /*U*/
      { "lui", "{}{}0110111" } };

  std::unordered_map<std::string, std::int32_t> m_immediate_length_map{
      { "addi", 12 }, { "lui", 20 }, { "lw", 12 },  { "sw", 7 },
      { "jalr", 12 }, { "jal", 20 }, { "beq", 13 }, { "bne", 13 },
      { "blt", 13 },  { "bge", 13 } };

  std::vector<std::shared_ptr<assembler::Operand>> m_operands;

 public:
  Instruction() = default;
  Instruction( const std::string& raw_instr ) : m_raw_instr( raw_instr ) {
    parseInstruction();
  }
  std::string_view getName() { return m_instr_name; }
  std::string_view getLabelName() { return m_label_name; }
  std::shared_ptr<assembler::Operand> getOperand( int index ) {
    return m_operands.at( index );
  }
  std::vector<std::shared_ptr<assembler::Operand>> getOperands() {
    return m_operands;
  }
  std::int32_t getOperandCount() { return m_operands.size(); }

  bool hasLabel() { return !m_label_name.empty(); }

  std::string getOPCode() { return m_opcode_map[m_instr_name]; }

  std::string getBinaryEncoding() {
    std::string binary_encoding;
    switch ( getOperandCount() ) {
      case 2:
        binary_encoding = encodeTwoOperandInstruction();
        break;
      case 3:
        binary_encoding = encodeThreeOperandInstruction();
        break;
    }
    return binary_encoding;
  }

  std::string encodeTwoOperandInstruction() {
    std::string encodedstring;

    if ( isLoadInstruction( m_instr_name ) ) {
      encodedstring = encodeLoad();
    } else if ( isStoreInstruction( m_instr_name ) ) {
      encodedstring = encodeStore();
    } else if ( isJalInstruction( m_instr_name ) ) {
      encodedstring = encodeJal();
    } else {
      encodedstring = fmt::format( m_opcode_map[m_instr_name],
                                   getOperand( 1 )->getBinaryValue(),
                                   getOperand( 0 )->getBinaryValue() );
    }
    return encodedstring;
  }

  std::string encodeLoad() {
    std::string encoded_string;

    RegisterDispOp* operand = (RegisterDispOp*)getOperand( 1 ).get();
    std::string register_bin = operand->getRegisterBinary();
    std::string disp_bin = operand->getDisplacementBinary();

    encoded_string =
        fmt::format( m_opcode_map[m_instr_name], disp_bin, register_bin,
                     getOperand( 0 )->getBinaryValue() );

    return encoded_string;
  }

  std::string encodeStore() {
    std::string encoded_string;

    RegisterDispOp* operand = (RegisterDispOp*)getOperand( 1 ).get();
    std::string register_bin = operand->getRegisterBinary();
    std::string disp_bin = operand->getDisplacementBinary();

    std::string offset1( disp_bin.rbegin(), disp_bin.rbegin() + 5 );
    std::reverse( offset1.begin(), offset1.end() );

    std::string offset2( disp_bin.rbegin() + 5, disp_bin.rbegin() + 12 );
    std::reverse( offset2.begin(), offset2.end() );

    encoded_string =
        fmt::format( m_opcode_map[m_instr_name], offset2,
                     getOperand( 0 )->getBinaryValue(), register_bin, offset1 );
    return encoded_string;
  }

  std::string encodeJal() {
    std::string encoded_string;

    std::string destination = getOperand( 0 )->getBinaryValue();

    auto dest_size = destination.size();

    std::string offset = getOperand( 1 )->getBinaryValue();

    auto offset_size = offset.size();

    std::string offset_p1;
    offset_p1.push_back( '0' );  // 20th bit (Trailing 0)
    std::string tmp1 =
        std::string( offset.rbegin() + 1, offset.rbegin() + 11 );  //[1:10]
    std::reverse( tmp1.begin(), tmp1.end() );                      // [10:1]
    offset_p1 += tmp1;
    offset_p1.push_back( *( offset.rbegin() + 11 ) );  // 11th bit

    auto offset_p1_size = offset_p1.size();

    std::string offset_p2( offset.rbegin() + 12, offset.rend() );  // [12:19]
    std::reverse( offset_p2.begin(), offset_p2.end() );            //[19:12]

    auto offset_p2_size = offset_p2.size();

    encoded_string = fmt::format( m_opcode_map[m_instr_name], offset_p1,
                                  offset_p2, destination );

    auto encoded_string_size = encoded_string.size();
    return encoded_string;
  }

  std::string encodeBranch() {
    std::string encoded_string;

    std::string rs1 = getOperand( 0 )->getBinaryValue();
    std::string rs2 = getOperand( 1 )->getBinaryValue();

    std::string offset = getOperand( 2 )->getBinaryValue();

    std::string offset_p1;
    offset_p1.push_back( *( offset.rbegin() + 12 ) );               // 12th bit
    std::string tmp1( offset.rbegin() + 5, offset.rbegin() + 11 );  // [5:10]
    std::reverse( tmp1.begin(), tmp1.end() );                       // [10:5]
    offset_p1 += tmp1;

    std::string offset_p2;
    std::string tmp2( offset.rbegin() + 1, offset.rbegin() + 5 );  // [1:4]
    std::reverse( tmp2.begin(), tmp2.end() );                      // [4:1]
    offset_p2 += tmp2;
    offset_p2.push_back( *( offset.rbegin() + 11 ) );  // 11th bit

    encoded_string = fmt::format( m_opcode_map[m_instr_name], offset_p1, rs2,
                                  rs1, offset_p2 );
    auto sz = encoded_string.size();
    return encoded_string;
  }

  std::string encodeThreeOperandInstruction() {
    std::string encoded_string;

    if ( isBranchInstruction( m_instr_name ) ) {
      encoded_string = encodeBranch();
    } else {
      encoded_string = fmt::format( m_opcode_map[m_instr_name],
                                    getOperand( 2 )->getBinaryValue(),
                                    getOperand( 1 )->getBinaryValue(),
                                    getOperand( 0 )->getBinaryValue() );
    }
    return encoded_string;
  }

 private:
  std::vector<std::string_view> getInstructionComponents() {
    std::vector<std::string_view> instruction_components;
    instruction_components =
        m_raw_instr | ranges::views::split( ' ' ) |
        ranges::views::transform( []( auto&& component ) {
          return std::string_view( &*component.begin(),
                                   ranges::distance( component ) );
        } ) |
        ranges::to_vector;
    return instruction_components;
  }

  void parseInstruction() {
    auto instruction_components = getInstructionComponents();
    for ( auto component : instruction_components ) {
      processComponent( component );
    }
  }

  void processComponent( std::string_view component ) {
    if ( isLabel( component ) ) {
      m_label_name = component;
      m_label_name.remove_suffix( 1 );

    } else if ( isInstruction( component ) ) {
      m_instr_name = component;

    } else {
      Operand::OperandType operand_type = Operand::identifyType( component );

      switch ( operand_type ) {
        case Operand::OperandType::RegisterOp:
          m_operands.emplace_back( new Register( component ) );
          break;
        case Operand::OperandType::LabelOp: {
          std::int32_t op_len = m_immediate_length_map.find( m_instr_name ) ==
                                        m_immediate_length_map.end()
                                    ? 12
                                    : m_immediate_length_map[m_instr_name];
          m_operands.emplace_back( new Label( component, op_len ) );
          break;
        }
        case Operand::OperandType::ImmediateOp:
          m_operands.emplace_back( new Immediate(
              std::stoll( std::string( component ) ),
              m_immediate_length_map[std::string( m_instr_name )] ) );
          break;
        case Operand::OperandType::RegisterDisplacementOp:
          m_operands.emplace_back( new RegisterDispOp( component ) );
          break;
      }
    }
  }

  bool isLabel( std::string_view str ) {
    return m_label_name.empty() && str.back() == ':';
  }
  bool isInstruction( std::string_view component ) {
    // You can put a different logic here
    return m_instr_name.empty();
  }

  bool isLoadInstruction( const std::string_view instr_name ) {
    return instr_name == "lw";
  }

  bool isStoreInstruction( const std::string_view instr_name ) {
    return instr_name == "sw";
  }
  bool isBranchInstruction( const std::string_view instr_name ) {
    return instr_name[0] == 'b';
  }

  bool isJalInstruction( const std::string_view instr_name ) {
    return instr_name == "jal";
  }
};
}  // namespace assembler