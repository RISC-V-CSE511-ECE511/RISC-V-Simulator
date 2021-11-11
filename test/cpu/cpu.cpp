#include <functional>

#include "cpu/decoder/connection_info.hpp"
#define BOOST_TEST_MODULE cpu_test

#include <boost/test/unit_test.hpp>
#include <cpu/decoder/decoder.hpp>
#include <string>

namespace cpu {

struct State {
  std::string memory = "";
  std::int32_t register_file[31] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0 };
  std::int32_t PC = 0;
  std::string IR = "";
};

struct Executor {
  // R Type
 private:
  static void add_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;
    rf[conn_info.operand1] = rf[conn_info.operand2] + rf[conn_info.operand3];
  }
  static void sub_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;
    rf[conn_info.operand1] = rf[conn_info.operand2] - rf[conn_info.operand3];
  }
  static void xor_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;
    rf[conn_info.operand1] = rf[conn_info.operand2] ^ rf[conn_info.operand3];
  }
  static void and_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;
    rf[conn_info.operand1] = rf[conn_info.operand2] & rf[conn_info.operand3];
  }
  static void or_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;
    rf[conn_info.operand1] = rf[conn_info.operand2] | rf[conn_info.operand3];
  }
  static void sll_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;
    rf[conn_info.operand1] = rf[conn_info.operand2] << rf[conn_info.operand3];
  }
  static void sra_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;
    rf[conn_info.operand1] = rf[conn_info.operand2] >> rf[conn_info.operand3];
  }

  // SB Type
  std::int32_t sext( const std::int32_t value,
                     const std::int32_t msb_position ) {
    std::int32_t result = value;

    // Extract 20 bit value
    std::int32_t msb_value = ( value & ( 1 << ( msb_position - 1 ) ) );

    // Check if the 20 bit is 1
    if ( msb_value ) {
      // Mask out bits after msb as 1
      std::int32_t mask = ( ~0 ) << msb_position;
      // Apply the mask
      result = result | mask;
      return value;
    }
    return value;  // Same value is returned in case the msb is 0
  }

 public:  // API
  static void execute( State& sys_state, const ConnectionInfo& conn_info ) {
    m_instr_func_map[conn_info.instr_name]( sys_state, conn_info );
  }

 private:
  static inline std::unordered_map<
      std::string, std::function<void( State&, const ConnectionInfo& )>>
      m_instr_func_map{ { "add", std::bind( &add_func, std::placeholders::_1,
                                            std::placeholders::_2 ) },
                        { "sub", std::bind( &sub_func, std::placeholders::_1,
                                            std::placeholders::_2 ) },
                        { "xor", std::bind( &xor_func, std::placeholders::_1,
                                            std::placeholders::_2 ) },
                        { "and", std::bind( &and_func, std::placeholders::_1,
                                            std::placeholders::_2 ) },
                        { "or", std::bind( &or_func, std::placeholders::_1,
                                           std::placeholders::_2 ) },
                        { "sll", std::bind( &sll_func, std::placeholders::_1,
                                            std::placeholders::_2 ) },
                        { "sra", std::bind( &sra_func, std::placeholders::_1,
                                            std::placeholders::_2 ) } };
};

struct CPU {
 private:
  State sys_state;

 public:
  void runProgram( const std::string& program ) {
    loadProgram( program );
    // Logic for this needs to be changed
    while ( sys_state.PC != sys_state.memory.size() ) {
      // fetch Instruction
      sys_state.IR = fetchInstruction();
      // Decode Instruction and Get Connection Information
      ConnectionInfo conn_info = Decoder::decode( sys_state.IR );
      // Execute Instruction
      Executor::execute( sys_state, conn_info );
    }
  }

  std::string fetchInstruction() {
    std::string instruction;
    instruction = std::string(
        sys_state.memory.begin() + sys_state.PC,
        sys_state.memory.begin() + sys_state.PC + 32 );  // Getting 4 bytes
    sys_state.PC += 32;
    return instruction;
  }

  void loadProgram( const std::string& program ) { sys_state.memory = program; }

  State& getSystemState() { return sys_state; }
};

}  // namespace cpu

using namespace cpu;

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

BOOST_AUTO_TEST_SUITE( r_type_test_suite )
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

BOOST_AUTO_TEST_SUITE_END()