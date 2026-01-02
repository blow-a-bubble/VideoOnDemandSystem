#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/async.h>
#include <iostream>
#include <memory>
#include <string>

#define TRACE__LOG(fmt, ...) bubble::default_logger->trace("[{:>10s}:{:<4d}] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define DEBUG__LOG(fmt, ...) bubble::default_logger->debug("[{:>10s}:{:<4d}] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define INFO__LOG(fmt, ...) bubble::default_logger->info("[{:>10s}:{:<4d}] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define WARN__LOG(fmt, ...) bubble::default_logger->warn("[{:>10s}:{:<4d}] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define ERROR__LOG(fmt, ...) bubble::default_logger->error("[{:>10s}:{:<4d}] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define CRITICAL__LOG(fmt, ...) bubble::default_logger->critical("[{:>10s}:{:<4d}] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
namespace bubble
{
    extern std::shared_ptr<spdlog::logger> default_logger;
    // 默认参数 true async.log debug
    void init_logger(bool debug_enable = true, const std::string &file = "async.log", spdlog::level::level_enum level = spdlog::level::level_enum::debug);
}
