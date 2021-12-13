#include <fmt/core.h>

#include <assembler/assembler.hpp>
#include <cpu/simulator.hpp>

#include "memory/common_enums.hpp"

std::string get_examples_dir() { return std::string( EXAMPLES ); }

int main() {
  // Invoking assembler to convert the assembly file to binary file
  assembler::turbo_asm engine( get_examples_dir() + "sample9.s" );
  std::string binary = engine.dumpBinary();

  // Passing that dump to the cpu for execution
  cpu::CPU test_cpu( 512, 512, 8, memory::CacheWritePolicy::WRITEBACK,
                     memory::CacheReplacementPolicy::LRU );
  test_cpu.runProgram( binary );
  test_cpu.getSystemState().dumpState();

  return 0;
}