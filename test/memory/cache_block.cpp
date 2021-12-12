#define BOOST_TEST_MODULE cache_block_tests

#include <boost/test/unit_test.hpp>
#include <memory/cache_block.hpp>

#include "boost/test/tools/old/interface.hpp"

using namespace memory;

BOOST_AUTO_TEST_SUITE( cache_block_test )

BOOST_AUTO_TEST_CASE( check_whether_address_is_present_in_cache ) {
  CacheBlock cache_block( 64 );

  cache_block.add_entry( 100, "Hello World" );
  cache_block.add_entry( 104, "What" );
  cache_block.add_entry( 108, "Bye" );
  cache_block.add_entry( 112, "Garbage" );
  cache_block.add_entry( 116, "Hmm" );

  std::int32_t test_address = 104;

  BOOST_ASSERT( cache_block.contains( test_address ) );
}

BOOST_AUTO_TEST_CASE( throw_if_more_data_than_block_size ) {
  CacheBlock cache_block( 20 );

  cache_block.add_entry( 100, "Hello World" );
  cache_block.add_entry( 104, "What" );
  cache_block.add_entry( 108, "Bye" );
  cache_block.add_entry( 112, "Garbage" );
  cache_block.add_entry( 116, "Hmm" );

  BOOST_REQUIRE_THROW( cache_block.add_entry( 120, "Hmm" ),
                       CacheBlock::CacheBlockException );
}

BOOST_AUTO_TEST_CASE( read_data ) {
  CacheBlock cache_block( 20 );

  cache_block.add_entry( 100, "Hello World" );
  cache_block.add_entry( 104, "What" );
  cache_block.add_entry( 108, "Bye" );
  cache_block.add_entry( 112, "Garbage" );
  cache_block.add_entry( 116, "Hmm" );

  std::int32_t test_address = 112;

  BOOST_REQUIRE_EQUAL( cache_block.read( test_address ), "Garbage" );
}

BOOST_AUTO_TEST_CASE( throw_on_reading_non_existent_data ) {
  CacheBlock cache_block( 20 );

  cache_block.add_entry( 100, "Hello World" );
  cache_block.add_entry( 104, "What" );
  cache_block.add_entry( 108, "Bye" );
  cache_block.add_entry( 112, "Garbage" );
  cache_block.add_entry( 116, "Hmm" );

  std::int32_t test_address = 150;

  BOOST_REQUIRE_THROW( cache_block.read( test_address ),
                       CacheBlock::CacheBlockException );
}

BOOST_AUTO_TEST_CASE( write_data_to_an_entry ) {
  CacheBlock cache_block( 20 );

  cache_block.add_entry( 100, "Hello World" );
  cache_block.add_entry( 104, "What" );
  cache_block.add_entry( 108, "Bye" );
  cache_block.add_entry( 112, "Garbage" );
  cache_block.add_entry( 116, "Hmm" );

  cache_block.write( 104, "Meow" );

  BOOST_ASSERT( cache_block.isDirty() );
  BOOST_REQUIRE_EQUAL( cache_block.read( 104 ), "Meow" );
}

BOOST_AUTO_TEST_SUITE_END()
