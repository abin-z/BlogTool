#include <fmt/core.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <CLI/CLI.hpp>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "fmt/base.h"
#include "inifile.h"
#include "spdlog/common.h"

namespace fs = std::filesystem;

#ifdef _WIN32
#include <windows.h>

void set_file_logger();

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

void config_hugo_path(const std::string& cfg_path, ini::inifile& config)
{
  fs::path hugo_path;
  do
  {
    std::string temp;
    fmt::println("未找到hugo可执行文件, 请输入hugo的安装路径:");
    std::getline(std::cin, temp);
    // 去掉可能的首尾引号
    if (!temp.empty() && temp.front() == '"' && temp.back() == '"')
    {
      temp = temp.substr(1, temp.size() - 2);
    }
    hugo_path = fs::path(temp);
  } while (!fs::exists(hugo_path) || !fs::is_regular_file(hugo_path));
  config["hugo"]["path"] = hugo_path.string();
  spdlog::info("已设置 Hugo 安装路径: {}", hugo_path.string());
  config.save(cfg_path);  // 保存配置文件
}

void config_typora_path(const std::string& cfg_path, ini::inifile& config)
{
  fs::path typora_path;
  do
  {
    std::string temp;
    fmt::println("未找到Typora可执行文件, 请输入Typora的安装路径:");
    std::getline(std::cin, temp);
    // 去掉可能的首尾引号
    if (!temp.empty() && temp.front() == '"' && temp.back() == '"')
    {
      temp = temp.substr(1, temp.size() - 2);
    }
    typora_path = fs::path(temp);
  } while (!fs::exists(typora_path) || !fs::is_regular_file(typora_path));
  config["typora"]["path"] = typora_path.string();
  spdlog::info("已设置 Typora 安装路径: {}", typora_path.string());
  config.save(cfg_path);  // 保存配置文件
}

void config_path(const std::string& cfg_path, ini::inifile& config)
{
  fmt::println("提示: 本软件运行依赖于hugo和typora, 请确保已安装并配置好环境变量!");
  config_hugo_path(cfg_path, config);
  config_typora_path(cfg_path, config);
}

// 业务逻辑统一函数, 用 UTF-8 编码参数
inline int run_cli(int argc, char** argv)
{
#ifdef _WIN32
  std::system("chcp 65001 > nul");  // 设置控制台编码为 UTF-8, 静默输出
  std::system("cls");
#else
  std::system("clear");
#endif
  set_file_logger();

  CLI::App app{"A CLI + fmt demo"};

  std::string name = "World";
  app.add_option("-n,--name", name, "Name to greet");
  CLI11_PARSE(app, argc, argv);
  spdlog::info("Hello, {}!", name);

  // 检测配置文件是否存在
  static constexpr char config_file[] = "blogtool.ini";
  ini::inifile config;
  if (!fs::exists(config_file) || !fs::is_regular_file(config_file))  // 配置文件不存在
  {
    spdlog::warn("配置文件不存在或无法加载: {}", config_file);
    config_path(config_file, config);
  }
  else
  {
    config.load(config_file);
    if (config.contains("hugo", "path"))
    {
      fs::path hugo_path = config["hugo"]["path"].as<std::string>();
      if (!fs::exists(hugo_path) || !fs::is_regular_file(hugo_path))
      {
        spdlog::warn("Hugo 安装路径无效: {}", hugo_path.string());
        config_hugo_path(config_file, config);
      }
    }
    if (config.contains("typora", "path"))
    {
      fs::path typora_path = config["typora"]["path"].as<std::string>();
      if (!fs::exists(typora_path) || !fs::is_regular_file(typora_path))
      {
        spdlog::warn("Typora 安装路径无效: {}", typora_path.string());
        config_typora_path(config_file, config);
      }
    }
  }

  fs::path hugo_path = config["hugo"]["path"].as<std::string>();
  fs::path typora_path = config["typora"]["path"].as<std::string>();
  spdlog::info("hugo 路径: {}", hugo_path.string());
  spdlog::info("typora 路径: {}", typora_path.string());

  // 请求输入博客名称
  std::string blog_name;
  do
  {
    spdlog::info("请输入要创建的博客标题: ");
    std::getline(std::cin, blog_name);
  } while (blog_name.empty());

  // 创建新的博客, 同时用 Typora 编辑器打开刚才创建的博客文件
  spdlog::info("正在创建博客: {}", blog_name);
  // TODO 需要到hugo目录下执行命令, 使用c++17的filesystem库来获取当前目录
  std::string command = fmt::format("\"{}\" new post/{}/index.md", hugo_path.string(), blog_name);
  spdlog::info("执行命令: {}", command);
  int ret = std::system(command.c_str());
  spdlog::info("命令执行结果: {}", ret);

  std::string typora_command = fmt::format("\"{}\"", typora_path.string());
  ret = std::system(typora_command.c_str());
  spdlog::info("Typora 打开结果: {} {}", ret, typora_command);

  return 0;
}

#ifdef _WIN32
int wmain(int argc, wchar_t** wargv)  // Windows 使用 wmain 入口点,保证输入中文输入不会崩溃
{
  // 转换宽字符参数到 UTF-8 字符串, 存储起来, 保证指针有效
  std::vector<std::string> utf8_args = convert_wargv_to_utf8_strings(argc, wargv);

  // 创建 char* 数组指针, 供 CLI11 使用
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

void set_file_logger()
{
  try
  {
    // 控制台 sink：只输出 info 以上
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::info);
    console_sink->set_pattern("[%^%Y-%m-%d %H:%M:%S.%e %L%$] %v");

    // 文件 sink：输出所有等级
    // // 基础文件 sink, false 表示追加模式
    // auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("blogtool_logs/blogtool.log", false);
    // file_sink->set_level(spdlog::level::trace);
    // file_sink->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");

    // 这里使用 daily_logger_mt 创建一个每天 00:00 新建日志文件的 logger
    auto daily_file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("blogtool_logs/blogtool.log", 0, 0);
    daily_file_sink->set_level(spdlog::level::trace);
    daily_file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
    // 创建 logger
    std::vector<spdlog::sink_ptr> sinks{daily_file_sink, console_sink};
    auto logger = std::make_shared<spdlog::logger>("multi_logger", sinks.begin(), sinks.end());

    // 设置为默认
    spdlog::set_default_logger(logger);

    // 设置 logger 总体等级（logger 不会转发低于该等级的日志给 sinks）
    logger->set_level(spdlog::level::trace);  // 所有日志等级都传给 sinks
    spdlog::flush_on(spdlog::level::warn);    // 设置 flush 条件为警告及以上等级

    // 测试
    spdlog::info("======================================");
  }
  catch (const spdlog::spdlog_ex& ex)
  {
    std::cerr << "日志初始化失败: " << ex.what() << std::endl;
  }
}