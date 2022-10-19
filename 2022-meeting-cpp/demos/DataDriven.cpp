#include <cassert>
#include <vector>
#include <sstream>






#include <clean-test/clean-test.h>

namespace ct = clean_test;
using namespace ct::literals;

auto str(auto && v) {
  auto buffer = std::ostringstream{};
  buffer << std::forward<decltype(v)>(v);
  return std::move(buffer).str();
}

auto const s = "data"_suite = [] {
  static auto const data = std::vector{0, 1337};
  ct::Test{"static", data} = [](int const n) {
    ct::expect(n > 0_i);
  };

  std::tuple{0, "1337"}
    | "temporary"_test = [](auto v) {
    ct::expect(str(v) != "1337"_sv);
  };
};


int main(int const argc, char const * const * const argv) {
  auto config = ct::Configuration::parse(argc, argv);
  config.m_num_jobs = 1; // sequential
  return ct::main(config);
}