#include <fmt/core.h>

#include <CLI/CLI.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "fmt/base.h"
#include "inifile.h"

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

  // 检测配置文件是否存在
  static constexpr char config_file[] = "blogtool.ini";
  ini::inifile config;
  if (!config.load(config_file))
  {
    fmt::print("无法加载配置文件: {}\n", config_file);
    return 1;  // 返回错误码
  }
  fmt::print("加载配置文件: {}\n", config_file);

  // 读取配置文件中的路径是否存在, hugo 和 typora 的路径, 不存在则提示指定路径(安装)


  // 请求输入博客名称


  // 创建新的博客, 同时用 Typora 编辑器打开刚才创建的博客文件


  fmt::print("Hello, {}!\n", name);
  fmt::print("请输入要创建的博客标题: \n");

  std::string title;
  std::getline(std::cin, title);
  if (title.empty())
  {
    fmt::print("标题不能为空，请重新输入。\n");
    return 1;  // 返回错误码
  }
  fmt::print("正在创建博客: {}\n", title);
  // TODO 需要到hugo目录下执行命令, 使用c++17的filesystem库来获取当前目录
  std::string command = fmt::format("hugo new post/{}/index.md", title);
  fmt::print("执行命令: {}\n", command);
  int ret = std::system("cmake --version");
  fmt::print("命令执行结果: {}\n", ret);

  //
  std::string typorapath = "typora.exe";

  ret = std::system("typora.exe");
  fmt::print("Typora 打开结果: {}\n", ret);

  return 0;
}

#ifdef _WIN32
int wmain(int argc, wchar_t** wargv)  // Windows 使用 wmain 入口点,保证输入中文输入不会崩溃
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
