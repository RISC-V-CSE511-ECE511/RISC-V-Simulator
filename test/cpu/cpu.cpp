#include <functional>
#define BOOST_TEST_MODULE cpu_test

#include <boost/test/unit_test.hpp>
#include <cpu/decoder/decoder.hpp>
#include <string>

namespace cpu {

struct Cpu {
 private:
  std::string m_memory = "";
  std::int32_t m_register_file[31] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0, 0, 0 };
  std::int32_t PC = 0;
  std::string IR = "";

 public:
  // R Type
  static void add_func( Cpu* cpu, const ConnectionInfo& conn_info ) {
    int* rf = cpu->getRegisterFile();
    rf[conn_info.operand1] = rf[conn_info.operand2] + rf[conn_info.operand3];
  }
  static void sub_func( Cpu* cpu, const ConnectionInfo& conn_info ) {
    int* rf = cpu->getRegisterFile();
    rf[conn_info.operand1] = rf[conn_info.operand2] - rf[conn_info.operand3];
  }
  static void xor_func( Cpu* cpu, const ConnectionInfo& conn_info ) {
    int* rf = cpu->getRegisterFile();
    rf[conn_info.operand1] = rf[conn_info.operand2] ^ rf[conn_info.operand3];
  }
  static void and_func( Cpu* cpu, const ConnectionInfo& conn_info ) {
    int* rf = cpu->getRegisterFile();
    rf[conn_info.operand1] = rf[conn_info.operand2] & rf[conn_info.operand3];
  }
  static void or_func( Cpu* cpu, const ConnectionInfo& conn_info ) {
    int* rf = cpu->getRegisterFile();
    rf[conn_info.operand1] = rf[conn_info.operand2] | rf[conn_info.operand3];
  }
  static void sll_func( Cpu* cpu, const ConnectionInfo& conn_info ) {
    int* rf = cpu->getRegisterFile();
    rf[conn_info.operand1] = rf[conn_info.operand2] << rf[conn_info.operand3];
  }
  static void sra_func( Cpu* cpu, const ConnectionInfo& conn_info ) {
    int* rf = cpu->getRegisterFile();
    rf[conn_info.operand1] = rf[conn_info.operand2] >> rf[conn_info.operand3];
  }

 private:
  static inline std::unordered_map<
      std::string, std::function<void( cpu::Cpu*, const ConnectionInfo& )>>
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

 public:
  int* getRegisterFile() { return m_register_file; }

  std::int32_t getPC() { return PC; }
  void setPC( std::int32_t new_PC ) { PC = new_PC; }

  std::string getIR() { return IR; }
  void setIR( std::string new_IR ) { IR = new_IR; }

  void runProgram( const std::string& program ) {
    loadProgram( program );
    // Logic for this needs to be changed
    while ( PC != m_memory.size() ) {
      // fetch Instruction
      IR = fetchInstruction();
      // Decode Instruction and Get Connection Information
      ConnectionInfo conn_info = Decoder::decode( IR );
      // Execute Instruction
      execute( conn_info );
    }
  }

  void execute( const ConnectionInfo& conn_info ) {
    m_instr_func_map[conn_info.instr_name]( this, conn_info );
  }

  std::string fetchInstruction() {
    std::string instruction;
    instruction = std::string( m_memory.begin() + PC,
                               m_memory.begin() + PC + 32 );  // Getting 4 bytes
    PC += 32;
    return instruction;
  }

  void loadProgram( const std::string& program ) { m_memory = program; }
};

}  // namespace cpu

using namespace cpu;

BOOST_AUTO_TEST_SUITE( cpu_test_suite )

BOOST_AUTO_TEST_CASE( fetch_instruction ) {
  Cpu test_cpu;
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
  Cpu test_cpu;
  std::string program = "00000000001100010000000010110011";

  int* rf = test_cpu.getRegisterFile();

  rf[2] = 10;
  rf[3] = 20;

  test_cpu.runProgram( program );

  BOOST_REQUIRE_EQUAL( rf[1], 30 );
}

BOOST_AUTO_TEST_CASE( sub_test ) {
  Cpu test_cpu;
  std::string program = "01000000001100010000000010110011";

  int* rf = test_cpu.getRegisterFile();

  rf[2] = 10;
  rf[3] = 20;

  test_cpu.runProgram( program );

  BOOST_REQUIRE_EQUAL( rf[1], -10 );
}

BOOST_AUTO_TEST_CASE( xor_test ) {
  Cpu test_cpu;
  std::string program = "00000000001100010100000010110011";

  int* rf = test_cpu.getRegisterFile();

  rf[2] = 10;
  rf[3] = 20;

  test_cpu.runProgram( program );

  BOOST_REQUIRE_EQUAL( rf[1], 30 );
}

BOOST_AUTO_TEST_CASE( and_test ) {
  Cpu test_cpu;
  std::string program = "00000000001100010111000010110011";

  int* rf = test_cpu.getRegisterFile();

  rf[2] = 10;
  rf[3] = 20;

  test_cpu.runProgram( program );

  BOOST_REQUIRE_EQUAL( rf[1], 0 );
}

BOOST_AUTO_TEST_CASE( or_test ) {
  Cpu test_cpu;
  std::string program = "00000000001100010110000010110011";

  int* rf = test_cpu.getRegisterFile();

  rf[2] = 10;
  rf[3] = 20;

  test_cpu.runProgram( program );

  BOOST_REQUIRE_EQUAL( rf[1], 30 );
}

BOOST_AUTO_TEST_CASE( sll_test ) {
  Cpu test_cpu;
  std::string program = "00000000001100010001000010110011";

  int* rf = test_cpu.getRegisterFile();

  rf[2] = 10;
  rf[3] = 20;

  test_cpu.runProgram( program );

  BOOST_REQUIRE_EQUAL( rf[1], 10485760 );
}

BOOST_AUTO_TEST_CASE( sra_test ) {
  Cpu test_cpu;
  std::string program = "01000000001100010101000010110011";

  int* rf = test_cpu.getRegisterFile();

  rf[2] = 10;
  rf[3] = 20;

  test_cpu.runProgram( program );

  BOOST_REQUIRE_EQUAL( rf[1], 0 );
}

BOOST_AUTO_TEST_SUITE_END()