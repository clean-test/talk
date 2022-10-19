#include <clean-test/clean-test.h>

namespace ct = clean_test;
using namespace ct::literals;

template <typename... Args>
constexpr bool use(Args&&...) {
  return true;
}




auto const all = "all"_test = [] {
int * p = nullptr;
ct::expect(not ct::lift(p) or *ct::lift(p)); // ok

ct::expect(not ct::lift(p) or use(*p)); // ERROR
// Alternative 1
if (p) {
  ct::expect(use(*p));
}
// Alternative 2: lifting callable
ct::expect(not ct::lift(p) or ct::lift([&] { return use(*p); }));
};
