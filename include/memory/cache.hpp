#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "main_memory.hpp"

namespace memory {
struct cache {
 private:
  std::int32_t size;
  std::int32_t miss_penalty;
  std::int32_t hit_time;
  std::int32_t block_size;
  std::string write_policy;
  std::string replacement_policy;

  // std::string associativity;              // Fully associative

  // static std::string query( int size, long address ) {}
};
}  // namespace memory
