#pragma once
#include <string>
#include <vector>
#include <algorithm>

#include "LowHeaders.h"

namespace LowEngine::Input {
    enum class ActionType {
        Keyboard,
        Mouse
    };

    class Action {
    public:
        std::string Name;

        ActionType Type;

        sf::Keyboard::Key Key = sf::Keyboard::Key::Unknown;
        sf::Mouse::Button MouseButton = sf::Mouse::Button::Left;

        bool LShift = false;
        bool LCtrl = false;
        bool LAlt = false;
        bool RShift = false;
        bool RCtrl = false;
        bool RAlt = false;

        bool Active = false;
        bool Started = false;
        bool Ended = false;

        [[nodiscard]] bool IsKeyPressed(const std::vector<sf::Keyboard::Key>& keys) const;
        [[nodiscard]] bool IsMouseButtonPressed(const std::vector<sf::Mouse::Button>& mouseButtons) const;
    };
}
