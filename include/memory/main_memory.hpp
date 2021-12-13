#pragma once

#include <algorithm>
#include <iterator>
#include <string>

namespace memory {
struct MainMemory {
  std::string m_raw_memory;

 public:
  MainMemory( const std::int32_t memory_size, const char fill_val )
      : m_raw_memory( memory_size * 8, fill_val ) {}

  std::string read( const std::int32_t address, const std::int32_t num_bytes ) {
    std::string data(
        m_raw_memory.begin() + translate( address ),
        m_raw_memory.begin() + translate( address ) + translate( num_bytes ) );
    return data;
  }

  void write( const std::int32_t address, const std::string& data ) {
    auto start_pos = m_raw_memory.begin() + translate( address );
    std::copy( data.begin(), data.end(), start_pos );
  }

  auto getSize() { return m_raw_memory.size(); }

 private:
  // Helpers
  std::int32_t translate( const std::int32_t address ) { return address * 8; }
};
}  // namespace memory
