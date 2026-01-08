#pragma once

#include <string_view>
#include <string>

namespace LowEngine::Utils {

    /**
     * @brief Retrieves the raw type name of a given type T as a string view.
     * 
     * This function uses compiler-specific macros to obtain the type name.
     * 
     * @tparam T The type whose name is to be retrieved.
     * @return A string view representing the raw type name of T.
	 */
    template<class T>
    constexpr std::string_view RawTypeName() {
#if defined(__clang__) || defined(__GNUC__)
        return __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
        return __FUNCSIG__;
#else
        return "Unsupported compiler";
#endif
    }

    /**
     * @brief Retrieves the cleaned type name of a given type T as a string view.
     * 
     * This function processes the raw type name to remove compiler-specific
     * prefixes and suffixes, returning only the relevant type information.
     * 
     * @tparam T The type whose cleaned name is to be retrieved.
	 * @return A string view representing the cleaned type name of T.
     */
    template<class T>
    constexpr std::string_view TypeName() {
        constexpr std::string_view s = RawTypeName<T>();
#if defined(__clang__)
        constexpr std::string_view prefix = "std::string_view LowEngine::Utils::RawTypeName() [T = ";
        constexpr std::string_view suffix = "]";
#elif defined(__GNUC__)
        constexpr std::string_view prefix = "constexpr std::string_view LowEngine::Utils::RawTypeName() [with T = ";
        constexpr std::string_view suffix = "]";
#elif defined(_MSC_VER)
        constexpr std::string_view prefix = "class std::basic_string_view<char,struct std::char_traits<char> > __cdecl LowEngine::Utils::RawTypeName<class ";
        constexpr std::string_view suffix = ">(void)";
#else
        constexpr std::string_view prefix = "";
        constexpr std::string_view suffix = "";
#endif
        const auto start = s.find(prefix);
        if (start == std::string_view::npos) return {};
        const auto content_start = start + prefix.size();
        const auto end = s.rfind(suffix);
        if (end == std::string_view::npos || end <= content_start) return {};
        return s.substr(content_start, end - content_start);
    }

    /**
     * @brief Retrieves the cleaned type name of a given type T as a std::string.
     * 
     * This function converts the cleaned type name string view to a std::string.
     * 
	 * @tparam T The type whose cleaned name is to be retrieved.
     */
    template<class T>
    inline std::string GetCleanTypeName() {
        return std::string(TypeName<T>());
    }
}