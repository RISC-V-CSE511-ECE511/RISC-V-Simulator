#include <cstdint>
#include <range/v3/action/reverse.hpp>
#include <unordered_map>
#define BOOST_TEST_MODULE assembler_test

#include <fmt/core.h>

#include <algorithm>
#include <assembler/immediate.hpp>
#include <assembler/label.hpp>
#include <assembler/operand.hpp>
#include <assembler/register.hpp>
#include <assembler/register_displacement.hpp>
#include <boost/test/unit_test.hpp>
#include <map>
#include <memory>
#include <range/v3/all.hpp>
#include <stdexcept>
#include <string>
#include <string_view>

// To be added in operand class
using namespace assembler;

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
    std::string op1_part1( op1.begin(), op1.begin() + 6 );
    std::string op1_part2( op1.begin() + 6, op1.end() );
    binary_encoding =
        fmt::format( m_opcode_map[m_instr_name],
                     getOperand( 0 )->getBinaryValue(), op1_part1, op1_part2 );
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
    fmt::print( offset );

    binary_encoding = fmt::format(
        m_opcode_map[m_instr_name],
        std::string( offset.begin(), offset.begin() + 5 ),
        getOperand( 0 )->getBinaryValue(), getOperand( 1 )->getBinaryValue(),
        std::string( offset.begin() + 5, offset.end() ) );

    std::string result = binary_encoding;
    return result;
  }
};

BOOST_AUTO_TEST_SUITE( instruction_methods )

BOOST_AUTO_TEST_CASE( get_name ) {
  Instruction test_instr( "add r1 r2 r3" );

  BOOST_REQUIRE_EQUAL( test_instr.getName(), "add" );
}
BOOST_AUTO_TEST_CASE( get_label_name ) {
  Instruction test_instr( "label: add r1 r2 r3" );

  BOOST_REQUIRE_EQUAL( test_instr.getLabelName(), "label" );
}
BOOST_AUTO_TEST_CASE( get_operand_out_of_bounds ) {
  Instruction test_instr( "label: add r1 r2 r3" );

  BOOST_REQUIRE_THROW( test_instr.getOperand( 100 ), std::out_of_range );
}
BOOST_AUTO_TEST_CASE( get_operand ) {
  Instruction test_instr( "label: add r1 r2 r3" );

  BOOST_REQUIRE_EQUAL(
      ( (Register*)test_instr.getOperand( 0 ).get() )->getName(), "r1" );
}
BOOST_AUTO_TEST_CASE( get_operand_count ) {
  Instruction test_instr( "label: add r1 r2 r3" );

  BOOST_REQUIRE_EQUAL( test_instr.getOperandCount(), 3 );
}
BOOST_AUTO_TEST_CASE( get_opcode_1 ) {
  Instruction test_instr( "sub r1 r2 r3" );

  BOOST_REQUIRE_EQUAL( test_instr.getOPCode(), "1100110{}000{}{}0000010" );
}
BOOST_AUTO_TEST_CASE( get_opcode_2 ) {
  Instruction test_instr( "addi r1 r2 r3" );

  BOOST_REQUIRE_EQUAL( test_instr.getOPCode(), "1100100{}000{}{}" );
}
BOOST_AUTO_TEST_CASE( get_opcode_3 ) {
  Instruction test_instr( "xor r1 r2 r3" );

  BOOST_REQUIRE_EQUAL( test_instr.getOPCode(), "1100110{}001{}{}0000000" );
}

BOOST_AUTO_TEST_CASE( get_binary_encoding_2_operand ) {
  Instruction test_instr( "lui r3 1024" );

  BOOST_REQUIRE_EQUAL( test_instr.getBinaryEncoding(),
                       "00000000010000000000000110110111" );
}

BOOST_AUTO_TEST_CASE( get_binary_encoding_3_operand ) {
  Instruction test_instr( "add r1 r2 r3" );

  BOOST_REQUIRE_EQUAL( test_instr.getBinaryEncoding(),
                       "00000000001100010000000010110011" );
}
BOOST_AUTO_TEST_CASE( sub_test ) {
  Instruction test_instr( "sub r1 r2 r3" );

  BOOST_REQUIRE_EQUAL( test_instr.getBinaryEncoding(),
                       "01000000001100010000000010110011" );
}
BOOST_AUTO_TEST_CASE( xor_test ) {
  Instruction test_instr( "xor r1 r2 r3" );

  BOOST_REQUIRE_EQUAL( test_instr.getBinaryEncoding(),
                       "00000000001100010100000010110011" );
}
BOOST_AUTO_TEST_CASE( and_test ) {
  Instruction test_instr( "and r1 r2 r3" );

  BOOST_REQUIRE_EQUAL( test_instr.getBinaryEncoding(),
                       "00000000001100010111000010110011" );
}
BOOST_AUTO_TEST_CASE( or_test ) {
  Instruction test_instr( "or r1 r2 r3" );

  BOOST_REQUIRE_EQUAL( test_instr.getBinaryEncoding(),
                       "00000000001100010110000010110011" );
}
BOOST_AUTO_TEST_CASE( addi_test ) {
  Instruction test_instr( "addi r1 r2 1024 " );

  BOOST_REQUIRE_EQUAL( test_instr.getBinaryEncoding(),
                       "01000000000000010000000010010011" );
}
BOOST_AUTO_TEST_CASE( sra_test ) {
  Instruction test_instr( "sra r1 r2 r3" );

  BOOST_REQUIRE_EQUAL( test_instr.getBinaryEncoding(),
                       "01000000001100010101000010110011" );
}
BOOST_AUTO_TEST_CASE( sll_test ) {
  Instruction test_instr( "sll r1 r2 r3" );

  BOOST_REQUIRE_EQUAL( test_instr.getBinaryEncoding(),
                       "00000000001100010001000010110011" );
}

BOOST_AUTO_TEST_CASE( beq_test ) {
  Instruction test_instr( "beq r1 r2 4" );

  BOOST_REQUIRE_EQUAL( test_instr.getBinaryEncoding(),
                       "00000000001000001000001001100011" );
}

BOOST_AUTO_TEST_CASE( bne_test ) {
  Instruction test_instr( "bne r1 r2 4" );

  BOOST_REQUIRE_EQUAL( test_instr.getBinaryEncoding(),
                       "00000000001000001001001001100011" );
}

BOOST_AUTO_TEST_CASE( blt_test ) {
  Instruction test_instr( "blt r1 r2 4" );

  BOOST_REQUIRE_EQUAL( test_instr.getBinaryEncoding(),
                       "00000000001000001100001001100011" );
}

BOOST_AUTO_TEST_CASE( bge_test ) {
  Instruction test_instr( "bge r1 r2 4" );

  BOOST_REQUIRE_EQUAL( test_instr.getBinaryEncoding(),
                       "00000000001000001101001001100011" );
}

BOOST_AUTO_TEST_SUITE_END()