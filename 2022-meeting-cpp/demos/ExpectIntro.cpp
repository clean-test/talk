#include <clean-test/clean-test.h>

namespace ct = clean_test;
using namespace ct::literals;

auto const t = "expect"_test = [] {
  ct::expect(true);
  ct::expect(0 == 7);
};