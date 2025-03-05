#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace LowEngine {
    extern std::shared_ptr<spdlog::logger> _log;
}