#define BOOST_TEST_MODULE cpu_test

#include <boost/test/unit_test.hpp>
#include <cpu/simulator.hpp>

using namespace cpu;

std::string get_examples_dir() { return std::string( EXAMPLES ); }

BOOST_AUTO_TEST_SUITE( cpu_test_suite )

BOOST_AUTO_TEST_CASE( fetch_instruction ) {
  CPU test_cpu;
  std::string program =
      "0000000000110001000000001011001101000000001100010000000010110011";
  test_cpu.loadProgram( program );

  std::string instruction1 = test_cpu.fetchInstruction();
  BOOST_REQUIRE_EQUAL( instruction1, "00000000001100010000000010110011" );

  std::string instruction2 = test_cpu.fetchInstruction();
  BOOST_REQUIRE_EQUAL( instruction2, "01000000001100010000000010110011" );
}
BOOST_AUTO_TEST_SUITE_END()
