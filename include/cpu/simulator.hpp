#pragma once

#include <algorithm>
#include <cpu/decoder/connection_info.hpp>
#include <cpu/decoder/decoder.hpp>
#include <cpu/executor..hpp>
#include <cpu/state.hpp>
#include <memory/memory_manager.hpp>
#include <string>

namespace cpu {

struct CPU {
 private:
  State sys_state;

 public:
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

  State& getSystemState() { return sys_state; }

  std::string fetchInstruction() {
    std::string instruction( sys_state.memory_manager.read( sys_state.PC, 4 ) );
    sys_state.PC += 4;
    sys_state.cycles_consumed += sys_state.memory_access_latency;
    return instruction;
  }

  void loadProgram( const std::string& program ) {
    sys_state.memory_manager.write( 0, program );
    sys_state.halt_adr =
        program.size() / 8;  // Will need to change for multiple program
    sys_state.total_instructions = program.size() / ( 32 * 8 );
  }
};

}  // namespace cpu