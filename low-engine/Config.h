#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

namespace LowEngine {
    class Config {
    public:
        inline static const char* LOGGER_NAME = "low_engine_spdlog_logger_name";
    };
}
