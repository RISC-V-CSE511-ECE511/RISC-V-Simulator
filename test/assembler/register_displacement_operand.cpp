#define BOOST_TEST_MODULE register_displacement_test

#include <assembler/register_displacement.hpp>
#include <boost/test/unit_test.hpp>

using namespace assembler;

BOOST_AUTO_TEST_SUITE( register_displacement_test )

BOOST_AUTO_TEST_CASE( get_register_name ) {
  RegisterDispOp test_operand( "8(r1)" );

  BOOST_REQUIRE_EQUAL( test_operand.getRegisterName(), "r1" );
}

BOOST_AUTO_TEST_CASE( get_displacement ) {
  RegisterDispOp test_operand( "8(r1)" );

  BOOST_REQUIRE_EQUAL( test_operand.getDisplacement(), 8 );
}

BOOST_AUTO_TEST_CASE( get_binary_value ) {
  RegisterDispOp test_operand( "8(r1)" );

  BOOST_REQUIRE_EQUAL( test_operand.getBinaryValue(), "" );
}

BOOST_AUTO_TEST_CASE( get_register_binary ) {
  RegisterDispOp test_operand( "8(r1)" );

  BOOST_REQUIRE_EQUAL( test_operand.getRegisterBinary(), "10000" );
}

BOOST_AUTO_TEST_CASE( get_displacement_binary ) {
  RegisterDispOp test_operand( "8(r1)" );

  BOOST_REQUIRE_EQUAL( test_operand.getDisplacementBinary(), "000100000000" );
}

BOOST_AUTO_TEST_SUITE_END()
