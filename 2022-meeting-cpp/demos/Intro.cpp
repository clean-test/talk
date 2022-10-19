#include <clean-test/clean-test.h>
#include <iostream>

namespace ct = clean_test;
using namespace ct::literals;

auto const talk = "talk"_suite = [] {
  "hello world"_test = [] {
    // Your tests go here
    std::cout << "Hello Meeting C++ 2022!\n";
  };
};

auto const intro = ct::Suite{"intro"} = [] {
  ct::Test{"demo", [] {
    std::cout << "Hi again\n";
  }};

  "unstable"_tag / "annoying"_test  = [] {
    std::cout << "Hi once more.\n";
  };
};