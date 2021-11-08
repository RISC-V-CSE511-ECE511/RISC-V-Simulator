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
      { "addi", "1100100{}000{}{}" },
      { "sub", "1100110{}000{}{}0000010" },
      { "add", "1100110{}000{}{}0000000" },
      { "jalr", "1110011{}000{}{}" },
      { "beq", "1100011{}000{}{}{}" },
      { "lw", "1100000{}010{}{}" },
      { "sw", "1100010{}010{}{}{}" },
      { "jal", "1111011{}{}{}" },
      { "bne", "1100011{}100{}{}{}" },
      { "sll", "1100110{}100{}{}0000000" },
      { "blt", "1100011{}001{}{}{}" },
      { "bge", "1100011{}101{}{}{}" },
      { "xor", "1100110{}001{}{}0000000" },
      { "lui", "1110110{}{}" },
      { "and", "1100110{}111{}{}0000000" },
      { "or", "1100110{}011{}{}0000000" },
      { "sra", "1100110{}101{}{}0000010" } };

  std::unordered_map<std::string, std::int32_t> m_immediate_length_map{
      { "addi", 12 }, { "lui", 20 }, { "lw", 12 },  { "sw", 7 },
      { "jalr", 12 }, { "jal", 12 }, { "beq", 12 }, { "bne", 12 },
      { "blt", 12 },  { "bge", 12 } };

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
  std::int32_t getOperandCount() { return m_operands.size(); }

  bool hasLabel() { return !m_label_name.empty(); }

  std::string getOPCode() { return m_opcode_map[m_instr_name]; }

  std::string getBinaryEncoding() {
    std::string binary_encoding;
    switch ( getOperandCount() ) {
      case 2:
        if ( isLoadInstruction( m_instr_name ) ) {
          binary_encoding = encodeLoad();
        } else if ( isStoreInstruction( m_instr_name ) ) {
          binary_encoding = encodeStore();
        } else {
          binary_encoding = fmt::format( m_opcode_map[m_instr_name],
                                         getOperand( 0 )->getBinaryValue(),
                                         getOperand( 1 )->getBinaryValue() );
        }
        break;
      case 3:

        if ( isBranchInstruction( m_instr_name ) ) {
          binary_encoding = encodeBranch();
        } else {
          binary_encoding = fmt::format( m_opcode_map[m_instr_name],
                                         getOperand( 0 )->getBinaryValue(),
                                         getOperand( 1 )->getBinaryValue(),
                                         getOperand( 2 )->getBinaryValue() );
        }
        break;
      case 4:
        binary_encoding = fmt::format( m_opcode_map[m_instr_name],
                                       getOperand( 0 )->getBinaryValue(),
                                       getOperand( 1 )->getBinaryValue(),
                                       getOperand( 2 )->getBinaryValue(),
                                       getOperand( 3 )->getBinaryValue() );
        break;
    }

    std::string result = binary_encoding;
    std::reverse( result.begin(), result.end() );
    return result;
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
        case Operand::OperandType::LabelOp:
          m_operands.emplace_back( new Label( component ) );
          break;
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

  std::string encodeStore() {
    std::string binary_encoding;
    std::string op1( static_cast<RegisterDispOp*>( getOperand( 1 ).get() )
                         ->getDisplacementBinary() );
    std::string op1_part1( op1.begin(), op1.begin() + 5 );
    std::string op1_part2( op1.begin() + 5, op1.end() );

    std::string register_binary =
        static_cast<RegisterDispOp*>( getOperand( 1 ).get() )
            ->getRegisterBinary();
    binary_encoding =
        fmt::format( m_opcode_map[m_instr_name], op1_part1, register_binary,
                     getOperand( 0 )->getBinaryValue(), op1_part2 );
    return binary_encoding;
  }

  std::string encodeLoad() {
    std::string binary_encoding;
    binary_encoding = fmt::format(
        m_opcode_map[m_instr_name], getOperand( 0 )->getBinaryValue(),
        static_cast<RegisterDispOp*>( getOperand( 1 ).get() )
            ->getRegisterBinary(),
        static_cast<RegisterDispOp*>( getOperand( 1 ).get() )
            ->getDisplacementBinary() );
    return binary_encoding;
  }

  std::string encodeBranch() {
    std::string binary_encoding;
    std::string offset = getOperand( 2 )->getBinaryValue();

    binary_encoding = fmt::format(
        m_opcode_map[m_instr_name],
        std::string( offset.begin(), offset.begin() + 5 ),
        getOperand( 0 )->getBinaryValue(), getOperand( 1 )->getBinaryValue(),
        std::string( offset.begin() + 5, offset.end() ) );

    std::string result = binary_encoding;
    return result;
  }
};
}  // namespace assembler