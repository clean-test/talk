#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(demo)

BOOST_AUTO_TEST_CASE(talk)
{
  BOOST_WARN(2 + 2 == 2 * 2);
  BOOST_CHECK_MESSAGE(
    3 + 3 == 2 * 3, "can't touch this");
  BOOST_REQUIRE_EQUAL(4 + 4, 2 * 4);
}

BOOST_AUTO_TEST_SUITE_END()