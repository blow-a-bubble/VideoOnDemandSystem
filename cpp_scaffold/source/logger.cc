#include "logger.h"
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
}
