#pragma once

#include <algorithm>
#include <cpu/decoder/connection_info.hpp>
#include <cpu/decoder/decoder.hpp>
#include <cpu/executor..hpp>
#include <cpu/state.hpp>
#include <string>

namespace cpu {

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
