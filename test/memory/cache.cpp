#define BOOST_TEST_MODULE cache_test

#include <bitset>
#include <boost/test/unit_test.hpp>
#include <memory/cache.hpp>

using namespace memory;

BOOST_AUTO_TEST_SUITE( cache_test )

// Read cache data

BOOST_AUTO_TEST_CASE( read_cache_data ) {
  std::string test_data =
      std::bitset<32>( std::numeric_limits<std::int32_t>::max() ).to_string();

  // Initialize main memory
  MainMemory main_memory( 512, '0' );
  main_memory.write( 100, test_data );

  // Connect main memory to cache
  CacheMemory cache_memory( main_memory, 12, 4 );

  // Put the data in cache
  cache_memory.read( 100 );

  // Try reading an address from cache
  // As there is nothing in cache fetch data from main memory and store in cache

  std::int32_t test_address = 100;

  // Read 4 bytes from location 100
  auto [data_present, result_data] = cache_memory.read( 100 );

  BOOST_ASSERT( data_present );
  BOOST_REQUIRE_EQUAL( test_data, result_data );
}

BOOST_AUTO_TEST_CASE( evict_cache_data ) {
  std::string test_data =
      std::bitset<32>( std::numeric_limits<std::int32_t>::max() ).to_string();

  // Initialize main memory
  MainMemory main_memory( 512, '0' );
  main_memory.write( 100, test_data );
  main_memory.write( 104, test_data );
  main_memory.write( 108, test_data );

  // Connect main memory to cache
  CacheMemory cache_memory( main_memory, 12, 4 );

  // Put the data in cache
  cache_memory.read( 100 );
  cache_memory.read( 104 );
  cache_memory.read( 100 );
  cache_memory.read( 108 );
  cache_memory.read( 112 );

  std::int32_t test_address = 100;

  // Read 4 bytes from location 100
  auto [data_present, result_data] = cache_memory.read( 100 );

  BOOST_ASSERT( !data_present );
  BOOST_REQUIRE_EQUAL( std::string( "" ), result_data );
}

BOOST_AUTO_TEST_CASE( write_data_to_existing_block ) {
  std::string test_data =
      std::bitset<32>( std::numeric_limits<std::int32_t>::max() ).to_string();

  MainMemory main_memory( 512, '0' );

  CacheMemory cache_memory( main_memory, 4, 4 );

  cache_memory.read( 100 );  // Bring data from adr 100 into cache

  cache_memory.write( 100, test_data );

  auto [data_present, result_data] = cache_memory.read( 100 );

  BOOST_ASSERT( data_present );

  BOOST_REQUIRE_EQUAL( result_data, test_data );
}

BOOST_AUTO_TEST_CASE( write_through_test ) {
  std::string test_data =
      std::bitset<32>( std::numeric_limits<std::int32_t>::max() ).to_string();

  MainMemory main_memory( 512, '0' );

  CacheMemory cache_memory( main_memory, 4, 4,
                            memory::CacheWritePolicy::WRITETHROUGH );

  cache_memory.read( 100 );  // Bring data from adr 100 into cache

  cache_memory.write( 100, test_data );

  auto [data_present, result_data] = cache_memory.read( 100 );

  BOOST_ASSERT( data_present );
  BOOST_REQUIRE_EQUAL( result_data, test_data );

  std::string main_mem_data = main_memory.read( 100, 4 );
  BOOST_REQUIRE_EQUAL( test_data, main_memory.read( 100, 4 ) );
}

BOOST_AUTO_TEST_CASE( write_back_test ) {
  std::string test_data =
      std::bitset<32>( std::numeric_limits<std::int32_t>::max() ).to_string();

  MainMemory main_memory( 512, '0' );

  CacheMemory cache_memory( main_memory, 4, 4,
                            memory::CacheWritePolicy::WRITEBACK );

  cache_memory.read( 100 );

  cache_memory.write( 100, test_data );

  BOOST_REQUIRE_NE( test_data, main_memory.read( 100, 4 ) );

  cache_memory.read( 104 );  // Eviction

  BOOST_REQUIRE_EQUAL( test_data, main_memory.read( 100, 4 ) );
}

BOOST_AUTO_TEST_SUITE_END()
