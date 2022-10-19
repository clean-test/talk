#include "Intro.cpp"

int main(int const argc, char const * const * const argv) {
  auto config = ct::Configuration::parse(argc, argv);
  config.m_operation = ct::execute::OperationMode::list;
  return ct::main(config);
}