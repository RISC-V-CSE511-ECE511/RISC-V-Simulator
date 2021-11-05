#define BOOST_TEST_MODULE assembler_test

#include <assembler/immediate.hpp>
#include <assembler/operand.hpp>
#include <assembler/register.hpp>
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
  std::unordered_map<std::string, std::string> opcode_map{
      { "addi", "1100100{}000{}{}" },
      { "sub", "1100110{}000{}{}0000010" },
      { "add", "1100110{}000{}{}0000000" },
      { "lw", "1100000{}010{}{}" },
      { "sw", "1100010{}010{}{}{}" },
      { "jalr", "1110011{}000{}{}" },
      { "jal", "1111011{}{}{}" },
      { "beq", "1100011{}000{}{}{}" },
      { "bne", "1100011{}100{}{}{}" },
      { "blt", "1100011{}001{}{}{}" },
      { "bge", "1100011{}001{}{}{}" },
      { "lui", "1110110{}{}" },
      { "and", "1100110{}111{}{}0000000" },
      { "or", "1100110{}011{}{}0000000" },
      { "xor", "1100110{}001{}{}0000000" },
      { "sll", "1100110{}100{}{}0000000" },
      { "sra", "1100110{}101{}{}0000010" } };

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

  std::string getOPCode() { return opcode_map[m_instr_name]; }

 private:
  std::vector<std::string_view> getInstructionComponents() {
    std::vector<std::string_view> instruction_components;
    instruction_components =
        m_raw_instr | ranges::view::split( ' ' ) |
        ranges::view::transform( []( auto&& component ) {
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
        case Operand::OperandType::ImmediateOp:
          // TODO: Operand length must be passed as per instruction ( Bits are
          // based on instr)
          m_operands.emplace_back(
              new Immediate( std::stoll( std::string( component ) ), 12 ) );
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

BOOST_AUTO_TEST_SUITE_END()