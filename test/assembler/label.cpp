#define BOOST_TEST_MODULE label_test

#include <assembler/label.hpp>
#include <boost/test/unit_test.hpp>
#include <cstddef>
#include <cstdint>
#include <range/v3/all.hpp>
#include <string>

using namespace assembler;

BOOST_AUTO_TEST_SUITE( label_test )

BOOST_AUTO_TEST_CASE( label_accessors ) {
  Label operand( "label" );

  operand.setAddress( 487 );

  BOOST_REQUIRE_EQUAL( operand.getAddress(), "487" );
}

BOOST_AUTO_TEST_CASE( label_get_name ) {
  Label operand( "label" );

  BOOST_REQUIRE_EQUAL( operand.getName(), "label" );
}

BOOST_AUTO_TEST_CASE( label_accessors_operand_length ) {
  Label operand( "label" );
  operand.setOperandLength( 7 );

  BOOST_REQUIRE_EQUAL( operand.getOperandLength(), 7 );
}

BOOST_AUTO_TEST_CASE( label_getBinaryValue_negative ) {
  Label operand( "label" );
  operand.setOperandLength( 7 );
  operand.setAddress( -63 );

  BOOST_REQUIRE_EQUAL( operand.getBinaryValue(), "1000001" );
}

BOOST_AUTO_TEST_CASE( label_getBinaryValue_positive ) {
  Label operand( "label" );
  operand.setOperandLength( 7 );
  operand.setAddress( 63 );

  BOOST_REQUIRE_EQUAL( operand.getBinaryValue(), "1111110" );
}

BOOST_AUTO_TEST_SUITE_END()
