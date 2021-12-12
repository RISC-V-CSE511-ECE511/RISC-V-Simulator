#pragma once

#include <list>
#include <memory/cache_block.hpp>

namespace memory {
struct FifoEvictor {
 private:
  std::list<CacheBlock>& m_cache_blocks;

 public:
  FifoEvictor( std::list<CacheBlock>& cache_blocks )
      : m_cache_blocks( cache_blocks ) {}
  CacheBlock evict() {
    CacheBlock evicted_block = m_cache_blocks.front();
    m_cache_blocks.pop_front();
    return evicted_block;
  }
};
}  // namespace memory