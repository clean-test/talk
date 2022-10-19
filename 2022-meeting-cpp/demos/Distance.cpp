#include <cassert>
#include <vector>
#include <sstream>






#include <clean-test/clean-test.h>

namespace ct = clean_test;
using namespace ct::literals;

auto const s = "distance"_suite = [] {
  "close"_test = [] {
static_assert(ct::utils::norm(-2) == 2_i);

auto const x = 0.15_d + 0.15;
auto const y = 0.1_d + 0.2;

ct::expect(x == y);
ct::expect(ct::is_close(x, y));
ct::expect(ct::is_close(x, y + .5));

ct::expect(
  ct::distance(x, y + .5) <= ct::tolerance(.5));
  };
};


int main(int const argc, char const * const * const argv) {
  auto config = ct::Configuration::parse(argc, argv);
  config.m_num_jobs = 1; // sequential
  return ct::main(config);
}
