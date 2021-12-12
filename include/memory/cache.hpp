#pragma once

#include <iterator>
#include <limits>
#include <list>
#include <memory/cache_block.hpp>
#include <memory/fifo_evictor.hpp>
#include <memory/lru_evictor.hpp>
#include <memory/main_memory.hpp>
#include <memory/random_evictor.hpp>

namespace memory {

struct CacheMemory {
  enum WritePolicy { WRITEBACK, WRITETHROUGH };
  enum ReplacementPolicy { FIFO, RANDOM, LRU };

 private:
  MainMemory& m_main_memory;
  std::int32_t m_cache_size;
  std::int32_t m_block_size;
  std::int32_t m_num_blocks;
  std::list<CacheBlock> cache_blocks;
  WritePolicy m_write_policy;
  ReplacementPolicy m_replacement_policy;
  FifoEvictor fifo_evictor;
  RandomEvictor random_evictor;
  lru_evictor lru_evictor;

 public:
  CacheMemory( MainMemory& main_memory, const std::int32_t cache_size,
               const std::int32_t block_size,
               WritePolicy write_policy = WritePolicy::WRITEBACK,
               ReplacementPolicy replacement_policy = ReplacementPolicy::FIFO )
      : m_main_memory( main_memory ),
        m_cache_size( cache_size ),
        m_block_size( block_size ),
        m_num_blocks( m_cache_size / m_block_size ),
        m_write_policy( write_policy ),
        m_replacement_policy( replacement_policy ),
        fifo_evictor( cache_blocks ),
        random_evictor( cache_blocks, m_num_blocks ),
        lru_evictor( cache_blocks ) {}

  std::pair<bool, std::string> read( const std::int32_t address ) {
    auto block_loc = find_block( address );

    if ( block_loc != cache_blocks.end() ) {
      return { true, block_loc->read( address ) };
    }

    // Reaching here means add the data to the cache
    update( address );

    if ( m_replacement_policy == ReplacementPolicy::LRU ) {
      lru_evictor.access( address );
    }

    return { false, {} };
  }

  bool write( const std::int32_t address, const std::string& data ) {
    auto block_loc = find_block( address );

    if ( block_loc != cache_blocks.end() ) {
      block_loc->write( address, data );

      if ( m_write_policy == WritePolicy::WRITETHROUGH ) {
        m_main_memory.write( address, data );
      }

      return true;
    }
    return false;
  }

 private:
  // Helpers
  std::list<CacheBlock>::iterator find_block( const std::int32_t address ) {
    auto loc = std::find_if( cache_blocks.begin(), cache_blocks.end(),
                             [address]( CacheBlock& cache_block ) {
                               return cache_block.contains( address );
                             } );

    return loc;
  }

  CacheBlock update( const std::int32_t address ) {
    std::string block_data = m_main_memory.read( address, m_block_size );
    CacheBlock cache_block = createCacheBlock( address, block_data );

    if ( cache_blocks.size() < m_num_blocks ) {
      cache_blocks.push_back( cache_block );
    } else {
      evict_block();
      cache_blocks.push_back( cache_block );
    }

    return cache_block;
  }

  CacheBlock createCacheBlock( const std::int32_t address,
                               const std::string& block_data ) {
    CacheBlock cache_block( m_block_size );

    for ( auto i = 0; i < m_block_size / 4; i += 4 ) {
      cache_block.add_entry( address + i, block_data.substr( i * 8, 32 ) );
    }

    return cache_block;
  }

  void evict_block() {
    CacheBlock evicted_block = removeBlock();

    if ( evicted_block.isDirty() && m_write_policy == WritePolicy::WRITEBACK ) {
      auto starting_address = evicted_block.getStartingAddress();
      auto block_data = evicted_block.get_raw_block_data();

      m_main_memory.write( starting_address, block_data );
    }
  }

  CacheBlock removeBlock() {
    switch ( m_replacement_policy ) {
      case FIFO:
        return fifo_evictor.evict();
        break;
      case RANDOM:
        return random_evictor.evict();
        break;
      case LRU:
        return lru_evictor.evict();
        break;
    }
  }
};

}  // namespace memory
