#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <optional>

#include "Action.h"
#include "SFML/Window/Event.hpp"

namespace LowEngine::Input {
    /**
     * @brief Represents the state of modifier keys such as Shift, Control, and Alt.
     *
     * This structure is used to track the state of both left and right modifier keys,
     * including Left Shift, Left Control, Left Alt, Right Shift, Right Control, and Right Alt.
     * Each key state is represented as a boolean value, indicating whether the key is pressed or not.
     */
    struct ModifierKey {
        bool LShift = false;
        bool LCtrl = false;
        bool LAlt = false;
        bool RShift = false;
        bool RCtrl = false;
        bool RAlt = false;
    };

    inline bool operator==(const Action& action, const ModifierKey& modifier) {
        return action.LShift == modifier.LShift &&
               action.LCtrl == modifier.LCtrl &&
               action.LAlt == modifier.LAlt &&
               action.RShift == modifier.RShift &&
               action.RCtrl == modifier.RCtrl &&
               action.RAlt == modifier.RAlt;
    }

    inline bool operator==(const ModifierKey& modifier, const Action& action) {
        return action == modifier;
    }

    inline bool operator!=(const Action& action, const ModifierKey& modifier) {
        return action.LShift != modifier.LShift ||
               action.LCtrl != modifier.LCtrl ||
               action.LAlt != modifier.LAlt ||
               action.RShift != modifier.RShift ||
               action.RCtrl != modifier.RCtrl ||
               action.RAlt != modifier.RAlt;
    }

    inline bool operator!=(const ModifierKey& modifier, const Action& action) {
        return action != modifier;
    }

    /**
     * @brief Represents an action key with associated modifier states.
     *
     * This structure is used to define a key along with the state of left and right modifier keys, including Shift, Control, and Alt.
     * It allows for more precise input functionality by combining a specific key press with additional modifier keys.
     *
     * The primary key is defined using an `sf::Keyboard::Key`, and modifiers are represented as individual boolean values
     * for left and right Shift, Control, and Alt keys. Instances of this structure can be constructed with or without specifying the key
     * and modifier states.
     */
    struct ActionKey {
        sf::Keyboard::Key Key = sf::Keyboard::Key::Unknown;
        bool LShift = false;
        bool LCtrl = false;
        bool LAlt = false;
        bool RShift = false;
        bool RCtrl = false;
        bool RAlt = false;

        ActionKey() = default;

        ActionKey(sf::Keyboard::Key key, bool lShift, bool lCtrl, bool lAlt, bool rShift, bool rCtrl, bool rAlt)
            : Key(key), LShift(lShift), LCtrl(lCtrl), LAlt(lAlt), RShift(rShift), RCtrl(rCtrl), RAlt(rAlt) {
        }

        ActionKey(sf::Keyboard::Key key, const ModifierKey& modifiers) : ActionKey(
            key, modifiers.LShift, modifiers.LCtrl, modifiers.LAlt, modifiers.RShift, modifiers.RCtrl, modifiers.RAlt) {
        }
    };

    inline bool operator==(const ActionKey& key, const ActionKey& other) {
        return key.Key == other.Key &&
               key.LShift == other.LShift &&
               key.LCtrl == other.LCtrl &&
               key.LAlt == other.LAlt &&
               key.RShift == other.RShift &&
               key.RCtrl == other.RCtrl &&
               key.RAlt == other.RAlt;
    }

    inline bool operator==(const ActionKey& key, const ModifierKey& modifier) {
        return key.LShift == modifier.LShift &&
               key.LCtrl == modifier.LCtrl &&
               key.LAlt == modifier.LAlt &&
               key.RShift == modifier.RShift &&
               key.RCtrl == modifier.RCtrl &&
               key.RAlt == modifier.RAlt;
    }

    inline bool operator==(const ModifierKey& modifier, const ActionKey& key) {
        return key == modifier;
    }
}

namespace std {
    /**
     * @brief Hash specialization for ActionKey, required by unordered_map
     */
    template<>
    struct hash<LowEngine::Input::ActionKey> {
        std::size_t operator()(const LowEngine::Input::ActionKey& k) const noexcept {
            return std::hash<int>()(static_cast<int>(k.Key)) ^ // Hash based on sf::Keyboard::Key
                   (std::hash<bool>()(k.LShift) << 1) ^
                   (std::hash<bool>()(k.LCtrl) << 2) ^
                   (std::hash<bool>()(k.LAlt) << 3) ^
                   (std::hash<bool>()(k.RShift) << 4) ^
                   (std::hash<bool>()(k.RCtrl) << 5) ^
                   (std::hash<bool>()(k.RAlt) << 6);
        }
    };
}

namespace LowEngine::Input {
    /**
     * @brief Responsible for monitoring input events and maintaining status of defined Actions.
     */
    class InputManager {
    public:
        InputManager() = default;

        ~InputManager() = default;

        /**
         * @brief Add new Action that should be maintained.
         * @tparam Keys List of modifier keys. Needs to be of type sf::Keyboard::Key
         * @param actionName Name of the new Action.
         * @param key Key that should be assigned to this Action
         * @param modifiers Modifier keys that should apply, if any
         */
        template<typename... Keys>
        void AddAction(const std::string& actionName, sf::Keyboard::Key key, Keys... modifiers) {
            static_assert((std::is_same_v<sf::Keyboard::Key, Keys> && ...),
                          "All modifiers must be of type sf::Keyboard::Key");

            Action newAction;
            newAction.Name = actionName;
            newAction.Type = ActionType::Keyboard;

            newAction.Key = key;
            ApplyKeyboardModifiers(newAction, {modifiers...});

            _actions[actionName] = std::move(newAction);
        };

        /**
         * @brief Add new Action that should be maintained.
         * @tparam Keys List of modifier keys. Needs to be of type sf::Keyboard::Key
         * @param actionName Name of the new Action.
         * @param mouseButton Mouse button that should be assigned to this Action
         * @param modifiers Modifier keys that should apply, if any
         */
        template<typename... Keys>
        void AddAction(const std::string& actionName, sf::Mouse::Button mouseButton, Keys... modifiers) {
            static_assert((std::is_same_v<sf::Keyboard::Key, Keys> && ...),
                          "All modifiers must be of type sf::Keyboard::Key");

            Action newAction;
            newAction.Name = actionName;
            newAction.Type = ActionType::Mouse;

            newAction.MouseButton = mouseButton;
            ApplyKeyboardModifiers(newAction, {modifiers...});

            _actions[actionName] = std::move(newAction);
        };

        /**
         * @brief Retrieve current position of mouse pointer on the window.
         *
         * Position is relative to upper-left corner of the window.
         * @return Current mouse position.
         */
        sf::Vector2i GetMousePosition();

        /**
         * @brief Retrieve defined Action by its name.
         *
         * Retrieved Action can be used to read its current state.
         * @param actionName Name of the action.
         * @return Action with provided name. Returns nullptr if Action with name doesn't exist.
         */
        const Action* GetAction(const std::string& actionName) const;

        /**
         * @brief INTERNAL: Clear current state for all Actions.
         */
        void ClearActionState();

        /**
         * @brief INTERNAL: Read input event from SFML.
         * @param event Current event.
         */
        void Read(const std::optional<sf::Event>& event);

        void Update();

    protected:
        bool _inputChanged = false;
        std::unordered_map<std::string, Action> _actions;

        std::vector<sf::Keyboard::Key> _currentKeys;
        std::vector<sf::Mouse::Button> _currentMouseButtons;
        sf::Vector2i _currentMousePosition;
        ModifierKey _currentModifiers;

        /**
         * @brief Apply provided modifiers to the Action.
         * @param action Action that should have modifiers applied.
         * @param modifierKeys Modifiers to apply.
         */
        void ApplyKeyboardModifiers(Action& action, const std::vector<sf::Keyboard::Key>& modifierKeys);
    };
}
