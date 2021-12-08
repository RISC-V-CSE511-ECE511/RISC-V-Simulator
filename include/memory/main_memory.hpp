#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace memory {
struct main_memory {
 private:
  std::int32_t access_time;
  std::int32_t m[256][1];
};
}  // namespace memory
