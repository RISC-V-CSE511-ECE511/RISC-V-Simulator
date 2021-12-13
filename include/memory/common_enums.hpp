#pragma once

namespace memory {
enum CacheWritePolicy { WRITEBACK, WRITETHROUGH };
enum CacheReplacementPolicy { FIFO, RANDOM, LRU };
}  // namespace memory