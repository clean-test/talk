#include <cassert>








#include <clean-test/clean-test.h>

namespace ct = clean_test;
using namespace ct::literals;

constexpr auto on_linux() {
  return true;
}

auto const s0 = "advanced"_suite = [] {

  "debug output"_test = [] {
ct::expect(false) << "but why?";
  };

  "asserted"_test = [] {
ct::expect(false) << ct::asserted;
ct::expect(1 / 0 == 42) << "not executed";
  };



  "flaky"_test = [] {
ct::expect(false) << ct::flaky_if(on_linux());

  };


  "throw"_test = [] {
ct::expect(not ct::throws([] {}));
ct::expect(not ct::throws<std::exception>(
  [] { throw 1; }
));
  };




  "abort"_test = [] {
ct::expect(ct::debug_aborts([] {
  assert(false);
}));
  };

};




int main(int const argc, char const * const * const argv) {
  auto config = ct::Configuration::parse(argc, argv);
  config.m_num_jobs = 1; // sequential
  return ct::main(config);
}