#define BOOST_TEST_MODULE memory_test

#include <assert.h>

#include <bitset>
#include <boost/test/unit_test.hpp>
#include <cassert>
#include <cstdlib>
#include <limits>
#include <memory/main_memory.hpp>

std::int32_t cnvAdr( const std::int32_t address ) { return address * 8; }

using namespace memory;

BOOST_AUTO_TEST_SUITE( memory_test_suite )

BOOST_AUTO_TEST_CASE( memory_test_read_write_behaviour ) {
  std::string data =
      std::bitset<32>( std::numeric_limits<std::uint32_t>::max() ).to_string();

  MainMemory main_memory( 512, '0' );

  main_memory.write( 100, data );

  BOOST_REQUIRE_EQUAL( data, main_memory.read( 100, 4 ) );
}

BOOST_AUTO_TEST_SUITE_END()