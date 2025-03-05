#pragma once
#include <string>

#include "LowHeaders.h"

namespace LowEngine::Input {
    class Action {
    public:
        std::string Name;

        sf::Keyboard::Key Key = sf::Keyboard::Key::LSystem;
        bool Shift = false;
        bool Ctrl = false;
        bool Alt = false;

        bool Active = false;
        bool Started = false;
        bool Ended = false;
    };
}
