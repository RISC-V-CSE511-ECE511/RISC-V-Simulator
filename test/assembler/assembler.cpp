#define BOOST_TEST_MODULE assembler_test

#include <assembler/assembler.hpp>
#include <boost/test/unit_test.hpp>
using namespace assembler;

std::string get_examples_dir() { return std::string( EXAMPLES ); }

BOOST_AUTO_TEST_SUITE( assembler_test )

BOOST_AUTO_TEST_CASE( assembler_test_1 ) {
  turbo_asm asm_engine( get_examples_dir() + "sample1.s" );
  asm_engine.dumpBinary( "sample1.txt" );

  std::fstream file( "sample1.txt" );
  std::stringstream file_stream;
  file_stream << file.rdbuf();

  BOOST_REQUIRE_EQUAL( file_stream.str(), "00000000001000001000001001100011" );
}

BOOST_AUTO_TEST_CASE( assembler_test_2 ) {
  turbo_asm asm_engine( get_examples_dir() + "sample2.s" );
  asm_engine.dumpBinary( "sample2.txt" );

  std::fstream file( "sample2.txt" );
  std::stringstream file_stream;
  file_stream << file.rdbuf();

  BOOST_REQUIRE_EQUAL(
      file_stream.str(),
      "0000000000110001000000001011001101000000001100010000000010110011" );
}

BOOST_AUTO_TEST_SUITE_END()