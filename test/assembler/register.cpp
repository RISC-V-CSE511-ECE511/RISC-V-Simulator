#define BOOST_TEST_MODULE register_operand_test

#include <assembler/register.hpp>
#include <boost/test/unit_test.hpp>

using namespace assembler;

BOOST_AUTO_TEST_SUITE( register_operand_methods )

BOOST_AUTO_TEST_CASE( accessors_test ) {
  Register test_register;
  test_register.setName( "r5" );

  BOOST_REQUIRE_EQUAL( test_register.getName(), "r5" );
}

BOOST_AUTO_TEST_CASE( getBinaryValueTest ) {
  Register test_register;
  test_register.setName( "r5" );

  BOOST_REQUIRE_EQUAL( test_register.getBinaryValue(), "00101" );
}

BOOST_AUTO_TEST_SUITE_END()