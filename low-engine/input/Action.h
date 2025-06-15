#pragma once
#include <string>
#include <vector>
#include <algorithm>

#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/Mouse.hpp"

namespace LowEngine::Input {
    /**
     * @brief Represents different types of actions that can be performed.
     */
    enum class ActionType {
        Keyboard,
        Mouse
    };

    /**
     * @brief Describe a named Action that abstracts input system.
     */
    class Action {
    public:
        /**
         * @brief Name of this Action.
         */
        std::string Name;

        /**
         * @brief Type of this Action.
         */
        ActionType Type;

        /**
         * @brief Keyboard key that should trigger this action.
         *
         * Only applicable if Action's Type is set to ActionType::Keyboard.
         */
        sf::Keyboard::Key Key = sf::Keyboard::Key::Unknown;
        /**
         * @brief Mouse button that should trigger this action.
         *
         * Only applicable if Action's Type is set to ActionType::Mouse.
         */
        sf::Mouse::Button MouseButton = sf::Mouse::Button::Left;

        bool LShift = false;
        bool LCtrl = false;
        bool LAlt = false;
        bool RShift = false;
        bool RCtrl = false;
        bool RAlt = false;

        /**
         * @brief Action is currently Active?
         *
         * This is true as long as Action's key is held down.
         */
        bool Active = false;

        /**
         * @brief Action just started.
         *
         * This is true only on Update during which key was pressed.
         */
        bool Started = false;

        /**
         * @brief Action just ended.
         *
         * This is true only on Update during which key was released.
         */
        bool Ended = false;

        /**
         * @brief Check if provided collection of keys include Key for this Action.
         * @param keys Collection of keys to check against.
         * @return True if Key for this action is currently pressed. False otherwise.
         */
        [[nodiscard]] bool IsKeyPressed(const std::vector<sf::Keyboard::Key>& keys) const;

        /**
         * @brief Check if provided collection of mouse buttons include MauseButton for this Action.
         * @param mouseButtons Collection of mouse buttons to check against.
         * @return True if MouseButton for this action is currently pressed. False otherwise.
         */
        [[nodiscard]] bool IsMouseButtonPressed(const std::vector<sf::Mouse::Button>& mouseButtons) const;
    };
}
