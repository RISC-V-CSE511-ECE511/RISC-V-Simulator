#pragma once

#include <fmt/core.h>
#include <fmt/format.h>

#include <cpu/state.hpp>
#include <iostream>
#include <memory/common_enums.hpp>
#include <memory/memory_manager.hpp>
#include <string>
#include <vector>

namespace cpu {

struct State : public StateData {
  memory::MemoryManager memory_manager;

  State( std::int32_t memory_size, std::int32_t cache_size,
         std::int32_t block_size, memory::CacheWritePolicy write_policy,
         memory::CacheReplacementPolicy replacement_policy )
      : memory_manager( memory_size, cache_size, block_size, write_policy,
                        replacement_policy, (StateData*)this ) {}

  void dumpState() {
    fmt::print( "Main Memory Size : {}\n",
                memory_manager.getMainMemory().getSize() );
    fmt::print( "Total Instructions : {}\n", total_instructions );
    for ( auto i = 0; i < instr_cycles_consumed.size(); i++ ) {
      fmt::print( "\t Instr {} : {}\n", i, instr_cycles_consumed[i] );
    }
    fmt::print( "Memory access latency : {}\n", memory_access_latency );
    fmt::print( "Decode time : {}\n", decode_time );
    fmt::print( "Execute time : {}\n", execute_time );
    fmt::print( "Cache Stats\n-----------------\n" );
    fmt::print( "Cache Size : {}\n",
                memory_manager.getCacheMemory().getCacheSize() );
    fmt::print( "Block Size : {}\n",
                memory_manager.getCacheMemory().getBlockSize() );
    fmt::print( "Num Blocks : {}\n",
                memory_manager.getCacheMemory().getNumofBlocks() );
    fmt::print( "Replacement Policy : {}\n",
                memory_manager.getCacheMemory().getReplacementPolicy() );
    fmt::print( "WritePolicy : {}\n",
                memory_manager.getCacheMemory().getWritePolicy() );

    fmt::print( "Cache Hit Time : {}\n", cache_hit_time );
    fmt::print( "Cache Miss Time : {}\n", cache_miss_penalty );

    fmt::print( "Cache Hits : {}\n", cache_hits );
    fmt::print( "Cache Misses : {}\n", cache_miss );

    fmt::print( "\nRegister File Info : \n------------\n" );

    for ( auto i = 0; i < 32; i++ ) {
      fmt::print( "\tRegister r{} : {}\n", i, register_file[i] );
    }
  }
};
}  // namespace cpu