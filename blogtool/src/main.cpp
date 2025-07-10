#ifdef _WIN32
#include <windows.h>

#include <string>
#include <vector>

#endif

#include <fmt/core.h>

#include <CLI/CLI.hpp>
#include <cstdlib>

// UTF-16 wchar_t* 转 UTF-8 std::string
std::string utf16_to_utf8(const std::wstring& wstr)
{
  int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), NULL, 0, NULL, NULL);
  std::string strTo(size_needed, 0);
  WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
  return strTo;
}

// 将 wmain 参数转换成 UTF-8 char* 数组
std::vector<char*> convert_wargv_to_argv(int argc, wchar_t** wargv, std::vector<std::string>& storage)
{
  storage.clear();
  storage.reserve(argc);
  std::vector<char*> argv;

  for (int i = 0; i < argc; ++i)
  {
    std::string arg = utf16_to_utf8(wargv[i]);
    storage.push_back(std::move(arg));
  }
  for (int i = 0; i < argc; ++i)
  {
    argv.push_back(const_cast<char*>(storage[i].c_str()));
  }
  return argv;
}

#ifdef _WIN32
int wmain(int argc, wchar_t** wargv)
{
  SetConsoleOutputCP(CP_UTF8);
  std::system("cls");

  std::vector<std::string> utf8_args;
  std::vector<char*> argv = convert_wargv_to_argv(argc, wargv, utf8_args);

  CLI::App app{"A CLI + fmt demo"};

  std::string name = "World";
  app.add_option("-n,--name", name, "Name to greet");

  CLI11_PARSE(app, argc, argv.data());

  fmt::print("Hello, {}!\n", name);

  return 0;
}
#else
int main(int argc, char** argv)
{
  std::system("clear");

  CLI::App app{"A CLI + fmt demo"};

  std::string name = "World";
  app.add_option("-n,--name", name, "Name to greet");

  CLI11_PARSE(app, argc, argv);

  fmt::print("Hello, {}!\n", name);

  return 0;
}
#endif
