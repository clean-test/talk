#include <clean-test/clean-test.h>

namespace ct = clean_test;
using namespace ct::literals;

auto const t = "expect"_test = [] {
  auto v = 1;
  auto * p = &v;
  ct::expect(nullptr == ct::lift(p));
  ct::expect(v == 7_i);
};