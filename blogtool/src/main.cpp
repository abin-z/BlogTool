#include <fmt/core.h>

#include <CLI/CLI.hpp>
#include <cstdlib>

// TODO: 解决中文输入崩溃问题
// TODO: git submodule 无效问题

int main(int argc, char** argv)
{
#ifdef _WIN32
  std::system("chcp 65001 > nul");  // 设置控制台编码为 UTF-8，静默输出
  std::system("cls");    // Windows 清屏
#else
  std::system("clear");  // Linux/macOS 清屏
#endif

  CLI::App app{"A CLI + fmt demo"};

  std::string name = "World";
  app.add_option("-n,--name", name, "Name to greet"); // TODO -n 输入中文会崩溃

  CLI11_PARSE(app, argc, argv);

  fmt::print("Hello, {}!\n", name);
  return 0;
}
