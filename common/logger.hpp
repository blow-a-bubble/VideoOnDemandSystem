#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/async.h>
#include <iostream>
#include <memory>
#include <string>
namespace bubble
{
    std::shared_ptr<spdlog::logger> default_logger;
    void init_logger(bool debug_enable, const std::string &file, spdlog::level::level_enum level)
    {
        spdlog::init_thread_pool(32768, 1);
        spdlog::flush_on(level);
        if (debug_enable)
            default_logger = spdlog::stdout_color_mt("default-logger");
        else
            default_logger = spdlog::basic_logger_mt<spdlog::async_factory>("default-logger", file);
        default_logger->set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
        default_logger->set_level(level);
    }

#define TRACE__LOG(fmt, ...) default_logger->trace("[{:>10s}:{:<4d}] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define DEBUG__LOG(fmt, ...) default_logger->debug("[{:>10s}:{:<4d}] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define INFO__LOG(fmt, ...) default_logger->info("[{:>10s}:{:<4d}] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define WARN__LOG(fmt, ...) default_logger->warn("[{:>10s}:{:<4d}] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define ERROR__LOG(fmt, ...) default_logger->error("[{:>10s}:{:<4d}] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define CRITICAL__LOG(fmt, ...) default_logger->critical("[{:>10s}:{:<4d}] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
}
