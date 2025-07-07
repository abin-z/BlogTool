#include <fmt/core.h>

#include <CLI/CLI.hpp>

int main(int argc, char** argv)
{
  CLI::App app{"A CLI + fmt demo"};

  std::string name = "World";
  app.add_option("-n,--name", name, "Name to greet");

  CLI11_PARSE(app, argc, argv);

  fmt::print("Hello, {}!\n", name);
  return 0;
}
