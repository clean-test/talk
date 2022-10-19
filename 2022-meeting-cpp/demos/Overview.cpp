#include <clean-test/clean-test.h>

namespace ct = clean_test;
using namespace ct::literals;

auto const s0 = "math"_suite = [] {
  "integer"_test = [] {
    ct::expect(1_i + 2 == 3);
  };
};