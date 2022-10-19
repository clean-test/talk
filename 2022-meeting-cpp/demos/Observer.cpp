#include <clean-test/clean-test.h>

#include <future>
#include <thread>

namespace ct = clean_test;
using namespace ct::literals;












void async(auto run) { std::async(run).wait(); }

auto const t = "par"_test = [](ct::Observer & o) {
  ct::expect(true);
  ct::expect(o, true);
  async([&] {
    ct::expect(o, true);
    ct::expect(true); // WRONG
  });
  async([&] {
    auto const setup = ct::ObservationSetup{o};
    ct::expect(true); // now ok
  });
};
