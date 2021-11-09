#define BOOST_TEST_MODULE decoder_test

#include <boost/test/unit_test.hpp>
#include <cpu/decoder/decoder.hpp>

using namespace cpu;

BOOST_AUTO_TEST_SUITE( decoder_test_suite )

BOOST_AUTO_TEST_CASE( get_instruction_type ) {
  std::string instruction = "00000000001100010000000010110011";

  BOOST_REQUIRE_EQUAL( Decoder::getType( instruction ), InstructionType::R );
}

BOOST_AUTO_TEST_CASE( get_instruction_name_r ) {
  std::string instruction = "00000000001100010000000010110011";

  std::string instruction_name =
      Decoder::getInstructionName( instruction, InstructionType::R );

  BOOST_REQUIRE_EQUAL( instruction_name, "add" );
}
BOOST_AUTO_TEST_CASE( get_instruction_name_i ) {
  std::string instruction = "01000000000000010000000010010011";

  std::string instruction_name =
      Decoder::getInstructionName( instruction, InstructionType::I );

  BOOST_REQUIRE_EQUAL( instruction_name, "addi" );
}
BOOST_AUTO_TEST_CASE( get_instruction_name_s ) {
  std::string instruction = "00000000000100010010010000100011";

  std::string instruction_name =
      Decoder::getInstructionName( instruction, InstructionType::S );

  BOOST_REQUIRE_EQUAL( instruction_name, "sw" );
}

BOOST_AUTO_TEST_CASE( get_instruction_name_sb ) {
  std::string instruction = "00000000001000001000001001100011";

  std::string instruction_name =
      Decoder::getInstructionName( instruction, InstructionType::SB );

  BOOST_REQUIRE_EQUAL( instruction_name, "beq" );
}

BOOST_AUTO_TEST_CASE( get_instruction_name_u ) {
  std::string instruction = "00000000010000000000000110110111";

  std::string instruction_name =
      Decoder::getInstructionName( instruction, InstructionType::U );

  BOOST_REQUIRE_EQUAL( instruction_name, "lui" );
}

// R
BOOST_AUTO_TEST_CASE( get_operand1_r ) {
  std::string instruction = "00000000001100010000000010110011";

  BOOST_REQUIRE_EQUAL( Decoder::getOperand1( instruction, InstructionType::R ),
                       1 );
}
BOOST_AUTO_TEST_CASE( get_operand2_r ) {
  std::string instruction = "00000000001100010000000010110011";

  BOOST_REQUIRE_EQUAL( Decoder::getOperand2( instruction, InstructionType::R ),
                       2 );
}
BOOST_AUTO_TEST_CASE( get_operand3_r ) {
  std::string instruction = "00000000001100010000000010110011";

  BOOST_REQUIRE_EQUAL( Decoder::getOperand3( instruction, InstructionType::R ),
                       3 );
}

// I
BOOST_AUTO_TEST_CASE( get_operand1_i ) {
  std::string instruction = "01000000000000010000000010010011";

  std::int32_t operand1_num =
      Decoder::getOperand1( instruction, InstructionType::I );

  BOOST_REQUIRE_EQUAL( operand1_num, 1 );
}

BOOST_AUTO_TEST_CASE( get_operand2_i ) {
  std::string instruction = "01000000000000010000000010010011";

  std::int32_t operand2_num =
      Decoder::getOperand2( instruction, InstructionType::I );

  BOOST_REQUIRE_EQUAL( operand2_num, 2 );
}

BOOST_AUTO_TEST_CASE( get_operand3_i ) {
  std::string instruction = "01000000000000010000000010010011";

  std::int32_t operand3_num =
      Decoder::getOperand3( instruction, InstructionType::I );

  BOOST_REQUIRE_EQUAL( operand3_num, 1024 );
}

// S
BOOST_AUTO_TEST_CASE( get_operand1_s ) {
  std::string instruction = "00000000000100010010010000100011";

  std::int32_t operand1_num =
      Decoder::getOperand1( instruction, InstructionType::S );

  BOOST_REQUIRE_EQUAL( operand1_num, 1 );
}

BOOST_AUTO_TEST_CASE( get_operand2_s ) {
  std::string instruction = "00000000000100010010010000100011";

  std::int32_t operand2_num =
      Decoder::getOperand2( instruction, InstructionType::S );

  BOOST_REQUIRE_EQUAL( operand2_num, 2 );
}

BOOST_AUTO_TEST_CASE( get_operand3_s ) {
  std::string instruction = "00000000000100010010010000100011";

  std::int32_t operand3_num =
      Decoder::getOperand3( instruction, InstructionType::S );

  BOOST_REQUIRE_EQUAL( operand3_num, 8 );
}

// SB
BOOST_AUTO_TEST_CASE( get_operand1_sb ) {
  std::string instruction = "00000000001000001000001001100011";

  std::int32_t operand1_num =
      Decoder::getOperand1( instruction, InstructionType::SB );

  BOOST_REQUIRE_EQUAL( operand1_num, 1 );
}

BOOST_AUTO_TEST_CASE( get_operand2_sb ) {
  std::string instruction = "00000000001000001000001001100011";

  std::int32_t operand2_num =
      Decoder::getOperand2( instruction, InstructionType::SB );

  BOOST_REQUIRE_EQUAL( operand2_num, 2 );
}

BOOST_AUTO_TEST_CASE( get_operand3_sb ) {
  std::string instruction = "00000000001000001000001001100011";

  std::int32_t operand3_num =
      Decoder::getOperand3( instruction, InstructionType::SB );

  BOOST_REQUIRE_EQUAL( operand3_num, 4 );
}

// U
BOOST_AUTO_TEST_CASE( get_operand1_u ) {
  std::string instruction = "00000000010000000000000110110111";

  std::int32_t operand1_num =
      Decoder::getOperand1( instruction, InstructionType::U );

  BOOST_REQUIRE_EQUAL( operand1_num, 3 );
}

BOOST_AUTO_TEST_CASE( get_operand2_u ) {
  std::string instruction = "00000000010000000000000110110111";

  std::int32_t operand2_num =
      Decoder::getOperand2( instruction, InstructionType::U );

  BOOST_REQUIRE_EQUAL( operand2_num, 1024 );
}

BOOST_AUTO_TEST_CASE( get_operand3_u ) {
  std::string instruction = "00000000010000000000000110110111";

  std::int32_t operand3_num =
      Decoder::getOperand3( instruction, InstructionType::U );

  BOOST_REQUIRE_EQUAL( operand3_num, std::numeric_limits<std::int32_t>::max() );
}

// UJ
BOOST_AUTO_TEST_CASE( get_operand1_uj ) {
  std::string instruction = "00000000010000000000000011101111";

  std::int32_t operand1_num =
      Decoder::getOperand1( instruction, InstructionType::UJ );

  BOOST_REQUIRE_EQUAL( operand1_num, 1 );
}

BOOST_AUTO_TEST_CASE( get_operand2_uj ) {
  std::string instruction = "00000000010000000000000011101111";

  std::int32_t operand2_num =
      Decoder::getOperand2( instruction, InstructionType::UJ );

  BOOST_REQUIRE_EQUAL( operand2_num, 4 );
}

BOOST_AUTO_TEST_CASE( get_operand3_uj ) {
  std::string instruction = "00000000010000000000000011101111";

  std::int32_t operand3_num =
      Decoder::getOperand3( instruction, InstructionType::UJ );

  BOOST_REQUIRE_EQUAL( operand3_num, std::numeric_limits<std::int32_t>::max() );
}

BOOST_AUTO_TEST_SUITE_END()