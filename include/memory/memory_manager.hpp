#pragma once

#include <cpu/state_data.hpp>
#include <memory/cache.hpp>
#include <memory/main_memory.hpp>

namespace memory {
struct MemoryManager {
 private:
  memory::MainMemory main_memory;
  memory::CacheMemory cache_memory;
  cpu::StateData* sys_state;

 public:
  MemoryManager( const std::int32_t memory_size, const std::int32_t cache_size,
                 const std::int32_t block_size,
                 memory::CacheWritePolicy write_policy,
                 memory::CacheReplacementPolicy replacement_policy,
                 cpu::StateData* system_state )
      : main_memory( memory_size, '0' ),
        cache_memory( main_memory, cache_size, block_size, write_policy,
                      replacement_policy ),
        sys_state( system_state ) {}

  std::string read( const std::int32_t address, std::int32_t num_bytes ) {
    auto [data_present, data] = cache_memory.read( address );
    if ( !data_present ) {
      // For a miss mem_access_time + cache_miss_penalty
      sys_state->cycles_consumed +=
          sys_state->memory_access_latency + sys_state->cache_miss_penalty;
      sys_state->cache_miss += 1;
      return main_memory.read( address, num_bytes );
    }
    // For a hit cycles consumed will be
    sys_state->cycles_consumed += sys_state->cache_hit_time;
    sys_state->cache_hits += 1;
    return data;
  }

  void write( const std::int32_t address, const std::string& data ) {
    if ( !cache_memory.write( address, data ) ) {
      sys_state->cycles_consumed +=
          sys_state->memory_access_latency + sys_state->cache_miss_penalty;
      sys_state->cache_miss += 1;
      main_memory.write( address, data );
    }
  }

  auto getMainMemory() { return main_memory; }
  auto getCacheMemory() { return cache_memory; }
};
}  // namespace memory