#define BOOST_TEST_MODULE executor_test

#include <assembler/assembler.hpp>
#include <boost/test/unit_test.hpp>
#include <cpu/executor..hpp>
#include <cpu/simulator.hpp>

using namespace cpu;

std::string get_examples_dir() { return std::string( EXAMPLES ); }

BOOST_AUTO_TEST_SUITE( executor_test )

BOOST_AUTO_TEST_CASE( add_test ) {
  CPU test_cpu;
  std::string program = "00000000001100010000000010110011";

  int* rf = test_cpu.getSystemState().register_file;

  rf[2] = 10;
  rf[3] = 20;

  test_cpu.runProgram( program );

  BOOST_REQUIRE_EQUAL( rf[1], 30 );
}

BOOST_AUTO_TEST_CASE( sub_test ) {
  CPU test_cpu;
  std::string program = "01000000001100010000000010110011";

  int* rf = test_cpu.getSystemState().register_file;

  rf[2] = 10;
  rf[3] = 20;

  test_cpu.runProgram( program );

  BOOST_REQUIRE_EQUAL( rf[1], -10 );
}

BOOST_AUTO_TEST_CASE( xor_test ) {
  CPU test_cpu;
  std::string program = "00000000001100010100000010110011";

  int* rf = test_cpu.getSystemState().register_file;

  rf[2] = 10;
  rf[3] = 20;

  test_cpu.runProgram( program );

  BOOST_REQUIRE_EQUAL( rf[1], 30 );
}

BOOST_AUTO_TEST_CASE( and_test ) {
  CPU test_cpu;
  std::string program = "00000000001100010111000010110011";

  int* rf = test_cpu.getSystemState().register_file;

  rf[2] = 10;
  rf[3] = 20;

  test_cpu.runProgram( program );

  BOOST_REQUIRE_EQUAL( rf[1], 0 );
}

BOOST_AUTO_TEST_CASE( or_test ) {
  CPU test_cpu;
  std::string program = "00000000001100010110000010110011";

  int* rf = test_cpu.getSystemState().register_file;

  rf[2] = 10;
  rf[3] = 20;

  test_cpu.runProgram( program );

  BOOST_REQUIRE_EQUAL( rf[1], 30 );
}

BOOST_AUTO_TEST_CASE( sll_test ) {
  CPU test_cpu;
  std::string program = "00000000001100010001000010110011";

  int* rf = test_cpu.getSystemState().register_file;

  rf[2] = 10;
  rf[3] = 20;

  test_cpu.runProgram( program );

  BOOST_REQUIRE_EQUAL( rf[1], 10485760 );
}

BOOST_AUTO_TEST_CASE( sra_test ) {
  CPU test_cpu;
  std::string program = "01000000001100010101000010110011";

  int* rf = test_cpu.getSystemState().register_file;

  rf[2] = 10;
  rf[3] = 20;

  test_cpu.runProgram( program );

  BOOST_REQUIRE_EQUAL( rf[1], 0 );
}

// SB Type Test
BOOST_AUTO_TEST_CASE( beq_test ) {
  CPU test_cpu;
  assembler::turbo_asm engine( get_examples_dir() + "cpu_sample1.s" );
  std::string binary = engine.dumpBinary();

  int* rf = test_cpu.getSystemState().register_file;

  rf[2] = 10;
  rf[3] = -15;
  rf[4] = -5;
  rf[7] = 1;

  test_cpu.runProgram( binary );

  BOOST_REQUIRE_EQUAL( rf[5], 0 );
  BOOST_REQUIRE_EQUAL( rf[6], 2 );
}

BOOST_AUTO_TEST_CASE( blt_test ) {
  CPU test_cpu;
  assembler::turbo_asm engine( get_examples_dir() + "cpu_sample2.s" );
  std::string binary = engine.dumpBinary();

  int* rf = test_cpu.getSystemState().register_file;

  rf[2] = 10;
  rf[3] = -15;
  rf[4] = -5;
  rf[7] = 1;

  test_cpu.runProgram( binary );

  BOOST_REQUIRE_EQUAL( rf[5], 0 );
  BOOST_REQUIRE_EQUAL( rf[6], 2 );
}

BOOST_AUTO_TEST_CASE( bge_test ) {
  CPU test_cpu;
  assembler::turbo_asm engine( get_examples_dir() + "cpu_sample3.s" );
  std::string binary = engine.dumpBinary();

  int* rf = test_cpu.getSystemState().register_file;

  rf[2] = 10;
  rf[3] = -15;
  rf[4] = -5;
  rf[7] = 1;

  test_cpu.runProgram( binary );

  BOOST_REQUIRE_EQUAL( rf[5], 0 );
  BOOST_REQUIRE_EQUAL( rf[6], 2 );
}

BOOST_AUTO_TEST_CASE( bne_test ) {
  CPU test_cpu;
  assembler::turbo_asm engine( get_examples_dir() + "cpu_sample3.s" );
  std::string binary = engine.dumpBinary();

  int* rf = test_cpu.getSystemState().register_file;

  rf[2] = 10;
  rf[3] = -15;
  rf[4] = -5;
  rf[7] = 1;

  test_cpu.runProgram( binary );

  BOOST_REQUIRE_EQUAL( rf[5], 0 );
  BOOST_REQUIRE_EQUAL( rf[6], 2 );
}

BOOST_AUTO_TEST_CASE( bne_test_true ) {
  CPU test_cpu;
  assembler::turbo_asm engine( get_examples_dir() + "cpu_sample4.s" );
  std::string binary = engine.dumpBinary();

  int* rf = test_cpu.getSystemState().register_file;

  rf[2] = 10;
  rf[3] = -15;
  rf[4] = -5;
  rf[7] = 1;

  test_cpu.runProgram( binary );

  BOOST_REQUIRE_EQUAL( rf[5], 2 );
  BOOST_REQUIRE_EQUAL( rf[6], 2 );
}

// U
BOOST_AUTO_TEST_CASE( lui_test ) {
  CPU test_cpu;

  assembler::turbo_asm engine( get_examples_dir() + "cpu_sample5.s" );
  std::string binary = engine.dumpBinary();

  int* rf = test_cpu.getSystemState().register_file;

  test_cpu.runProgram( binary );

  BOOST_REQUIRE_EQUAL( rf[1], 4194304 );
}

// I Type
BOOST_AUTO_TEST_CASE( addi_test ) {
  CPU test_cpu;

  assembler::turbo_asm engine( get_examples_dir() + "cpu_sample6.s" );
  std::string binary = engine.dumpBinary();

  int* rf = test_cpu.getSystemState().register_file;

  test_cpu.runProgram( binary );

  BOOST_REQUIRE_EQUAL( rf[1], 1024 );
}

BOOST_AUTO_TEST_CASE( lw_test ) {
  CPU test_cpu;

  assembler::turbo_asm engine( get_examples_dir() + "cpu_sample7.s" );
  std::string binary = engine.dumpBinary();

  std::int32_t test_value = 1024;
  std::string value = std::bitset<32>( test_value ).to_string();
  std::reverse( value.begin(), value.end() );

  test_cpu.getSystemState().memory_manager.write( 4, value );

  // std::copy( value.begin(), value.end(),
  //            test_cpu.getSystemState().memory.begin() + 32 );

  int* rf = test_cpu.getSystemState().register_file;

  rf[2] = 2;

  test_cpu.runProgram( binary );

  BOOST_REQUIRE_EQUAL( rf[1], 1024 );
}

BOOST_AUTO_TEST_CASE( sw_test ) {
  CPU test_cpu;

  assembler::turbo_asm engine( get_examples_dir() + "cpu_sample8.s" );
  std::string binary = engine.dumpBinary();

  int* rf = test_cpu.getSystemState().register_file;

  rf[1] = 50;
  rf[2] = 8;

  test_cpu.runProgram( binary );

  BOOST_REQUIRE_EQUAL( rf[3], 50 );
}

BOOST_AUTO_TEST_CASE( jal_test ) {
  CPU test_cpu;

  int* rf = test_cpu.getSystemState().register_file;

  std::string program =
      "00000000001100010000000010110011"
      "00000000100000000000001011101111"
      "00000000000100001000011001100011"
      "00000000001100001000000010110011"
      "00000000000000101000001001100111";

  rf[2] = 1;
  rf[3] = 1;

  test_cpu.runProgram( program );

  BOOST_REQUIRE_EQUAL( rf[1], 3 );
}

BOOST_AUTO_TEST_SUITE_END()
