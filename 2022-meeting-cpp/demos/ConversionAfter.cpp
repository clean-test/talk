#include <clean-test/clean-test.h>

namespace ct = clean_test;
using namespace ct::literals;

auto const demo = "demo"_suite = [] {

"talk"_test = []
{
  ct::expect(2_i + 2 == 2_i * 2) << ct::flaky;
  ct::expect(
    3_i + 3 == 2_i * 3) << "can't touch this";
  ct::expect(4_i + 4 == 2_i * 4) << ct::asserted;
};

};
