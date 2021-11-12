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
      // Reset cycles consumed for every new instruction
      sys_state.cycles_consumed = 0;
      // fetch Instruction
      sys_state.IR = fetchInstruction();
      // Decode Instruction and Get Connection Information
      auto [cycles_consumed, conn_info] = Decoder::decode( sys_state.IR );
      sys_state.cycles_consumed += cycles_consumed;
      // Execute Instruction
      Executor::execute( sys_state, conn_info );

      sys_state.instr_cycles_consumed.push_back( sys_state.cycles_consumed );
    }
  }

  std::string fetchInstruction() {
    std::string instruction;
    instruction = std::string(
        sys_state.memory.begin() + sys_state.PC,
        sys_state.memory.begin() + sys_state.PC + 32 );  // Getting 4 bytes
    sys_state.PC += 32;
    sys_state.cycles_consumed += sys_state.memory_access_latency;
    return instruction;
  }

  void loadProgram( const std::string& program ) {
    std::copy( program.begin(), program.end(), sys_state.memory.begin() );
    sys_state.halt_adr =
        program.size();  // Will need to change for multiple program
    sys_state.total_instructions = program.size() / ( 32 * 8 );
  }

  void initializeMemory( std::int32_t mem_size ) {
    sys_state.memory = std::string( mem_size, '0' );
  }

  State& getSystemState() { return sys_state; }
};

}  // namespace cpu
