#include "Log.h"

namespace LowEngine {
    // Define the static shared pointer. Instance created in Game's constructor
    std::shared_ptr<spdlog::logger> _log = nullptr;
    fmt::memory_buffer _logContent;

    std::string DemangledTypeName(const std::type_index& type) {
        const char* typeName = type.name(); // MSVC name is demangled by default
#if defined(__GNUC__) || defined(__clang__) || defined(__MINGW32__) || defined(__MINGW64__)
        // Use GCC/Clang's ABI demangling
        int status;
        char* demangledName = abi::__cxa_demangle(typeName, nullptr, nullptr, &status);
        if (status == 0 && demangledName != nullptr) {
            std::string result = demangledName;
            free(demangledName);
            return result;
        }
#endif
        return typeName; // Return the original name if demangling is not supported or failed
    }
}
