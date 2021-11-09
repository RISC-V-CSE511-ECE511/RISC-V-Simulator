#define BOOST_TEST_MODULE immediate_operand

#include <assembler/immediate.hpp>
#include <boost/test/unit_test.hpp>

using namespace assembler;

BOOST_AUTO_TEST_SUITE( immediate_operand_methods )

BOOST_AUTO_TEST_CASE( get_value ) {
  Immediate test_operand( 487 );

  BOOST_REQUIRE_EQUAL( test_operand.getValue(), 487 );
}

BOOST_AUTO_TEST_CASE( get_operand_length ) {
  Immediate test_operand( 0, 12 );

  BOOST_REQUIRE_EQUAL( test_operand.getOperandLength(), 12 );
}

BOOST_AUTO_TEST_CASE( get_binary_value ) {
  Immediate test_operand( 5, 20 );

  BOOST_REQUIRE_EQUAL( test_operand.getBinaryValue(), "00000000000000000101" );
}

BOOST_AUTO_TEST_SUITE_END()