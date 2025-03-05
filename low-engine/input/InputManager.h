#pragma once

#include "Action.h"
#include "LowHeaders.h"

namespace LowEngine::Input {
    struct Key {
        sf::Keyboard::Key key;
        bool shift;
        bool ctrl;
        bool alt;
    };

    class InputManager {
    public:
        void AddAction(const std::string& actionName, sf::Keyboard::Key key, bool shift = false, bool ctrl = false, bool alt = false);

        Action& GetAction(const std::string& actionName);

    protected:
        std::unordered_map<std::string, Action> _actions;
    };
}
