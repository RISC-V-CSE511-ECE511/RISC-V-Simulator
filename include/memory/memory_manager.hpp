#pragma once

#include <memory/cache.hpp>
#include <memory/main_memory.hpp>

namespace memory {
struct MemoryManager {
  memory::MainMemory main_memory;
  memory::CacheMemory cache_memory;
  // Add cache as well

  MemoryManager()
      : main_memory( 512, '0' ),
        cache_memory( main_memory, 64, 4, CacheMemory::WRITEBACK,
                      CacheMemory::FIFO ) {}

  std::string read( const std::int32_t address, std::int32_t num_bytes ) {
    auto [data_present, data] = cache_memory.read( address );
    if ( !data_present ) {
      return main_memory.read( address, num_bytes );
    }
    return data;
  }

  void write( const std::int32_t address, const std::string& data ) {
    if ( !cache_memory.write( address, data ) ) {
      main_memory.write( address, data );
    }
  }
};
}  // namespace memory