#pragma once

#include <algorithm>
#include <deque>
#include <list>
#include <memory/cache_block.hpp>

namespace memory {

struct Lru_Evictor {
 private:
  std::list<CacheBlock>& m_cache_blocks;
  std::deque<std::int32_t> m_addresses;

 public:
  lru_evictor( std::list<CacheBlock>& cache_blocks )
      : m_cache_blocks( cache_blocks ) {}

  CacheBlock evict() {
    CacheBlock evicted_block = *( findBlock( m_addresses.front() ) );
    m_cache_blocks.erase( findBlock( m_addresses.front() ) );
    m_addresses.pop_front();
    return evicted_block;
  }

  void access( std::int32_t address ) {
    auto adr_present =
        std::find( m_addresses.begin(), m_addresses.end(), address );

    if ( adr_present != m_addresses.end() ) {
      m_addresses.erase( adr_present );
    }

    m_addresses.push_back( address );
  }

  std::list<CacheBlock>::iterator findBlock( std::int32_t address ) {
    for ( auto it = m_cache_blocks.begin(); it != m_cache_blocks.end(); it++ ) {
      if ( it->getStartingAddress() == address ) {
        return it;
      }
    }
    return m_cache_blocks.end();
  }
};
}  // namespace memory