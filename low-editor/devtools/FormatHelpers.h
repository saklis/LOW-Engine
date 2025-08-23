#pragma once

#include <string>

#include "SFML/System/Time.hpp"

namespace LowEngine {
    class FormatHelpers {
    public:
        static std::string ToString(const sf::Time& time);
    };
}
