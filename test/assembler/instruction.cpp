#define BOOST_TEST_MODULE assembler_test

#include <assembler/instruction.hpp>
#include <boost/test/unit_test.hpp>

using namespace assembler;

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

BOOST_AUTO_TEST_CASE( lw_test ) {
  Instruction test_instr( "lw r1 8(r2)" );

  BOOST_REQUIRE_EQUAL( test_instr.getBinaryEncoding(),
                       "00000000100000010010000010000011" );
}

BOOST_AUTO_TEST_CASE( sw_test ) {
  Instruction test_instr( "sw r1 8(r2)" );

  BOOST_REQUIRE_EQUAL( test_instr.getBinaryEncoding(),
                       "00000000000100010010010000100011" );
}

BOOST_AUTO_TEST_CASE( jal_test ) {
  Instruction test_instr( "jal r1 4" );

  BOOST_REQUIRE_EQUAL( test_instr.getBinaryEncoding(),
                       "00000000010000000000000011101111" );
}

BOOST_AUTO_TEST_CASE( sub_test_2 ) {
  Instruction test_instr( "sub r29 r30 r31" );

  BOOST_REQUIRE_EQUAL( test_instr.getBinaryEncoding(),
                       "01000001111111110000111010110011" );
}

BOOST_AUTO_TEST_SUITE_END()