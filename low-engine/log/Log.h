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
	 * @brief Memory buffer for log content. Holds the log messages in memory for display in the UI.
	 *
	 * Don't modify this buffer directly, use the logger to write messages!
	 */
	extern LOWENGINE_API fmt::memory_buffer _logContent;

	/**
	 * @brief Helper function to make type names readable.
	 * @param type The type index of the type to demangle.
	 */
	std::string DemangledTypeName(const std::type_index& type);

	/**
	 * @brief Formats a parameter pack of keys into a comma-separated string representation.
	 * @tparam Keys Types of keys (must be sf::Keyboard::Key)
	 * @param modifiers Parameter pack of modifier keys
	 * @return A formatted string representation of the keys as "[key1, key2, ...]"
	 */
	template <typename... Keys>
	std::string FormatKeysModifiers(Keys... modifiers) {
		std::string result = "[";

		// Using fold expression to concatenate all keys
		int index = 0;
		((result += (index++ > 0 ? ", " : "") + std::to_string(static_cast<int>(modifiers))), ...);

		return result + "]";
	}
}
