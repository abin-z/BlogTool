#include <fmt/core.h>

#include <CLI/CLI.hpp>
#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include "fmt/base.h"

#ifdef _WIN32
#include <windows.h>

// Windows 宽字符转 UTF-8
std::string utf16_to_utf8(const std::wstring& wstr)
{
  int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), NULL, 0, NULL, NULL);
  std::string strTo(size_needed, 0);
  WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
  return strTo;
}

// 宽字符 argv 转 UTF-8 argv
std::vector<std::string> convert_wargv_to_utf8_strings(int argc, wchar_t** wargv)
{
  std::vector<std::string> args;
  args.reserve(argc);
  for (int i = 0; i < argc; ++i)
  {
    args.push_back(utf16_to_utf8(wargv[i]));
  }
  return args;
}
#endif

// 业务逻辑统一函数，用 UTF-8 编码参数
inline int run_cli(int argc, char** argv)
{
#ifdef _WIN32
  std::system("chcp 65001 > nul");  // 设置控制台编码为 UTF-8，静默输出
  std::system("cls");
#else
  std::system("clear");
#endif

  CLI::App app{"A CLI + fmt demo"};

  std::string name = "World";
  app.add_option("-n,--name", name, "Name to greet");

  CLI11_PARSE(app, argc, argv);

  fmt::print("Hello, {}!\n", name);
  fmt::print("请输入要创建的博客标题: \n");

  std::string title;
  std::getline(std::cin, title);
  if (title.empty())
  {
    fmt::print("标题不能为空，请重新输入。\n");
    return 1; // 返回错误码
  }
  fmt::print("创建博客标题: {}\n", title);

  return 0;
}

#ifdef _WIN32
int wmain(int argc, wchar_t** wargv) // Windows 使用 wmain 入口点,保证输入中文输入不会崩溃
{
  // 转换宽字符参数到 UTF-8 字符串，存储起来，保证指针有效
  std::vector<std::string> utf8_args = convert_wargv_to_utf8_strings(argc, wargv);

  // 创建 char* 数组指针，供 CLI11 使用
  std::vector<char*> argv;
  argv.reserve(argc);
  for (auto& arg : utf8_args) argv.push_back(arg.data());

  return run_cli(argc, argv.data());
}
#else
int main(int argc, char** argv)
{
  return run_cli(argc, argv);
}
#endif
