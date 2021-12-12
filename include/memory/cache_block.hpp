#pragma once

#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace memory {
struct CacheBlock {
 private:
  bool m_is_dirty = false;
  std::unordered_map<std::int32_t, std::string> m_entries;
  std::int32_t m_blocksize;
  std::int32_t m_dataitems;

 public:
  class CacheBlockException : std::logic_error {
   public:
    CacheBlockException( const std::string& message )
        : std::logic_error( message ) {}
  };

  CacheBlock( const std::int32_t block_size )
      : m_blocksize( block_size ), m_dataitems( block_size / 4 ) {}

  void add_entry( const std::int32_t address, const std::string& value ) {
    if ( m_entries.size() == m_dataitems ) {
      throw CacheBlockException( "Data Items excedded than cache block size" );
    }

    m_entries[address] = value;
  }
  bool contains( const std::int32_t address ) {
    return m_entries.contains( address );
  }

  bool isDirty() { return m_is_dirty; }

  std::int32_t getStartingAddress() {
    auto sorted_addresses = getAddressesPresent();
    return *( sorted_addresses.begin() );
  }

  std::string get_raw_block_data() {
    std::string raw_block_data = "";
    auto sorted_addresses = getAddressesPresent();

    for ( auto address : sorted_addresses ) {
      raw_block_data += m_entries[address];
    }

    return raw_block_data;
  }

  std::string read( const std::int32_t address ) {
    if ( contains( address ) ) {
      return m_entries[address];
    }
    throw CacheBlockException( "Trying to read non-existent entry" );
  }
  void write( const std::int32_t address, const std::string& data ) {
    if ( contains( address ) ) {
      m_entries[address] = data;
      m_is_dirty = true;
      return;
    }
    throw CacheBlockException( "Writing to non-existent entry" );
  }

 private:
  std::set<std::int32_t> getAddressesPresent() {
    std::set<std::int32_t> addresses_present;
    for ( auto& [key, value] : m_entries ) {
      addresses_present.insert( key );
    }
    return addresses_present;
  }
};
}  // namespace memory
