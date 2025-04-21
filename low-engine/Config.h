#pragma once

#include <limits>

namespace LowEngine {
    class Config {
    public:
        inline static const std::size_t MAX_COMPONENTS = 1000;
        inline static const char* LOGGER_NAME = "low_engine_spdlog_logger_name";
        inline static const unsigned int MAX_SIZE = std::numeric_limits<std::size_t>::max();
    };
}
