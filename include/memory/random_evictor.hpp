#pragma once

#include <iterator>
#include <list>
#include <memory/cache_block.hpp>
#include <random>

namespace memory {
struct RandomEvictor {
 private:
  std::list<CacheBlock>& m_cache_blocks;
  std::int32_t m_num_blocks;

 public:
  RandomEvictor( std::list<CacheBlock>& cache_blocks, std::int32_t num_blocks )
      : m_cache_blocks( cache_blocks ), m_num_blocks( num_blocks ) {}

  CacheBlock evict() {
    long offset = generateRandomNumber();

    auto loc = m_cache_blocks.begin();
    std::advance( loc, offset );

    CacheBlock evicted_block = *loc;
    m_cache_blocks.erase( loc );

    return evicted_block;
  }

  std::int32_t generateRandomNumber() {
    std::random_device dev;
    std::mt19937 rng( dev() );
    std::uniform_int_distribution<std::mt19937::result_type> dist(
        0, m_num_blocks - 1 );

    return dist( rng );
  }
};
}  // namespace memory
