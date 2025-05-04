#pragma once

#include <typeindex>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <cxxabi.h>

namespace LowEngine {
    /**
     * @brief Logger instance for the engine.
     */
    extern std::shared_ptr<spdlog::logger> _log;

    /**
     * @brief Helper function to make type names readable.
     * @param type The type index of the type to demangle.
     */
    const char* DemangledTypeName(const std::type_index& type);
}
