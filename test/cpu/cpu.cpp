#include <unistd.h>

#include <functional>

#include "cpu/decoder/connection_info.hpp"
#define BOOST_TEST_MODULE cpu_test

#include <algorithm>
#include <assembler/assembler.hpp>
#include <bitset>
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
  std::int32_t halt_adr;
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

  static std::int32_t translateAddress( std::int32_t address ) {
    return address * 8;
  }

  static std::int32_t sext( const std::int32_t value,
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

  static void beq_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;

    std::int32_t rs1 = rf[conn_info.operand1];
    std::int32_t rs2 = rf[conn_info.operand2];
    std::int32_t offset = sext( conn_info.operand3, 12 );

    if ( rs1 == rs2 ) {
      sys_state.PC -= 32;  // Reverse the change made by fetch
      sys_state.PC += translateAddress( offset );
    }
  }

  static void blt_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;

    std::int32_t rs1 = rf[conn_info.operand1];
    std::int32_t rs2 = rf[conn_info.operand2];
    std::int32_t offset = sext( conn_info.operand3, 12 );

    if ( rs1 < rs2 ) {
      sys_state.PC -= 32;  // Reverse the change made by fetch
      sys_state.PC += translateAddress( offset );
    }
  }
  static void bge_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;

    std::int32_t rs1 = rf[conn_info.operand1];
    std::int32_t rs2 = rf[conn_info.operand2];
    std::int32_t offset = sext( conn_info.operand3, 12 );

    if ( rs1 >= rs2 ) {
      sys_state.PC -= 32;  // Reverse the change made by fetch
      sys_state.PC += translateAddress( offset );
    }
  }
  static void bne_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;

    std::int32_t rs1 = rf[conn_info.operand1];
    std::int32_t rs2 = rf[conn_info.operand2];
    std::int32_t offset = sext( conn_info.operand3, 12 );

    if ( rs1 != rs2 ) {
      sys_state.PC -= 32;  // Reverse the change made by fetch
      sys_state.PC += translateAddress( offset );
    }
  }

  // U Type Support
  static void lui_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;

    std::int32_t& rd = rf[conn_info.operand1];
    std::int32_t immediate = conn_info.operand2;

    immediate = ( immediate << 12 ) & ( ( ~0 ) << 12 );

    rd = sext( immediate, 32 );
  }

  // I Type Support
  static void addi_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;

    std::int32_t& rd = rf[conn_info.operand1];
    std::int32_t rs1 = rf[conn_info.operand2];
    std::int32_t immediate = sext( conn_info.operand3, 12 );

    rd = rs1 + immediate;
  }

  static void lw_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;

    std::int32_t& rd = rf[conn_info.operand1];
    std::int32_t rs1 = rf[conn_info.operand2];
    std::int32_t offset = sext( conn_info.operand3, 12 );

    rd = sext( loadFromMemory( sys_state.memory, rs1 + offset, 4 ), 32 );
  }

  // S Type
  static void sw_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;

    std::int32_t rs2 = rf[conn_info.operand1];
    std::int32_t rs1 = rf[conn_info.operand2];
    std::int32_t offset = sext( conn_info.operand3, 12 );

    storeToMemory( rs2, sys_state.memory, rs1 + offset );
  }

  static void jalr_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;

    std::int32_t& rd = rf[conn_info.operand1];
    std::int32_t rs1 = rf[conn_info.operand2];
    std::int32_t offset = sext( conn_info.operand3, 12 );

    std::int32_t t = sys_state.PC;
    sys_state.PC = rs1 + sext( translateAddress( offset ), 12 );
    rd = sys_state.PC;
  }

  static void jal_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;

    std::int32_t& rd = rf[conn_info.operand1];
    std::int32_t offset = sext( translateAddress( conn_info.operand2 ), 12 );

    rd = sys_state.PC;  // Already updated by fetch

    sys_state.PC += offset - 32;  // Already updated by fetch
  }

  static std::int32_t loadFromMemory( const std::string& mem,
                                      std::int32_t address,
                                      std::int32_t num_bytes_to_read ) {
    std::int32_t actual_address = address * 8;
    std::string val( mem.begin() + actual_address,
                     mem.begin() + actual_address + ( num_bytes_to_read * 8 ) );
    std::reverse( val.begin(), val.end() );
    return std::stoi( val, nullptr, 2 );
  }

  static void storeToMemory( std::int32_t value, std::string& mem,
                             std::int32_t address ) {
    std::int32_t actual_address = address * 8;
    std::string binary_value = std::bitset<32>( value ).to_string();
    std::reverse( binary_value.begin(), binary_value.end() );
    std::copy( binary_value.begin(), binary_value.end(),
               mem.begin() + actual_address );
  }

 public:  // API
  static void execute( State& sys_state, const ConnectionInfo& conn_info ) {
    m_instr_func_map[conn_info.instr_name]( sys_state, conn_info );
  }

 private:
  static inline std::unordered_map<
      std::string, std::function<void( State&, const ConnectionInfo& )>>
      m_instr_func_map{
          { "add", std::bind( &add_func, std::placeholders::_1,
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
                              std::placeholders::_2 ) },
          { "addi", std::bind( &addi_func, std::placeholders::_1,
                               std::placeholders::_2 ) },
          { "lw", std::bind( &lw_func, std::placeholders::_1,
                             std::placeholders::_2 ) },
          { "sw", std::bind( &sw_func, std::placeholders::_1,
                             std::placeholders::_2 ) },
          { "jalr", std::bind( &jalr_func, std::placeholders::_1,
                               std::placeholders::_2 ) },
          { "jal", std::bind( &jal_func, std::placeholders::_1,
                              std::placeholders::_2 ) },
          { "beq", std::bind( &beq_func, std::placeholders::_1,
                              std::placeholders::_2 ) },
          { "bne", std::bind( &bne_func, std::placeholders::_1,
                              std::placeholders::_2 ) },
          { "blt", std::bind( &blt_func, std::placeholders::_1,
                              std::placeholders::_2 ) },
          { "bge", std::bind( &bge_func, std::placeholders::_1,
                              std::placeholders::_2 ) },
          { "lui", std::bind( &lui_func, std::placeholders::_1,
                              std::placeholders::_2 ) },
          { "jal", std::bind( &jal_func, std::placeholders::_1,
                              std::placeholders::_2 ) },
          { "jalr", std::bind( &jalr_func, std::placeholders::_1,
                               std::placeholders::_2 ) },

      };
};

struct CPU {
 private:
  State sys_state;

 public:
  CPU() { initializeMemory( 1024 ); }
  void runProgram( const std::string& program ) {
    loadProgram( program );
    // Logic for this needs to be changed
    while ( sys_state.PC != sys_state.halt_adr ) {
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

  void loadProgram( const std::string& program ) {
    std::copy( program.begin(), program.end(), sys_state.memory.begin() );
    sys_state.halt_adr =
        program.size();  // Will need to change for multiple program
  }

  void initializeMemory( std::int32_t mem_size ) {
    sys_state.memory = std::string( mem_size, '0' );
  }

  State& getSystemState() { return sys_state; }
};

}  // namespace cpu

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
  std::copy( value.begin(), value.end(),
             test_cpu.getSystemState().memory.begin() + 32 );

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

BOOST_AUTO_TEST_SUITE_END()