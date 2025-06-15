#pragma once

#if defined(_MSC_VER)
	#ifdef LOWENGINE_EXPORTS
		#define LOWENGINE_API __declspec(dllexport)
	#else
		#define LOWENGINE_API __declspec(dllimport)
	#endif
#else
	#define LOWENGINE_API // GCC/Clang/MinGW export-all by default
#endif

#include <typeindex>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#if defined(__GNUC__) || defined(__clang__) || defined(__MINGW32__) || defined(__MINGW64__)
#include <cxxabi.h>
#endif

namespace LowEngine {
    /**
     * @brief Logger instance for the engine.
     */
    extern LOWENGINE_API std::shared_ptr<spdlog::logger> _log;

    /**
     * @brief Helper function to make type names readable.
     * @param type The type index of the type to demangle.
     */
    const char* DemangledTypeName(const std::type_index& type);
}
