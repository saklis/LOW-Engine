#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include "LowHeaders.h"

#include "Action.h"

namespace LowEngine::Input {
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
    class InputManager {
    public:
        InputManager() = default;

        ~InputManager() = default;

        template<typename... Keys>
        void AddAction(const std::string& actionName, sf::Keyboard::Key key, Keys... modifiers) {
            static_assert((std::is_same_v<sf::Keyboard::Key, Keys> && ...),
                          "All modifiers must be of type sf::Keyboard::Key");

            Action newAction;
            newAction.Name = actionName;
            newAction.Type = ActionType::Keyboard;

            newAction.Key = key;
            ApplyKeyboardModifiers(newAction, { modifiers... });

            _actions[actionName] = std::move(newAction);
        };

        template<typename... Keys>
        void AddAction(const std::string& actionName, sf::Mouse::Button mouseButton, Keys... modifiers) {
            static_assert((std::is_same_v<sf::Keyboard::Key, Keys> && ...),
                          "All modifiers must be of type sf::Keyboard::Key");

            Action newAction;
            newAction.Name = actionName;
            newAction.Type = ActionType::Mouse;

            ApplyKeyboardModifiers(newAction, { modifiers... });

            _actions[actionName] = std::move(newAction);

        };

        sf::Vector2i GetMousePosition();

        const Action& GetAction(const std::string& actionName) const;

        void ClearActionState();

        void Read(const std::optional<sf::Event>& event);

        void Update();

    protected:
        bool _inputChanged = false;

        /**
         * @var _emptyAction
         * @brief Represents a default or no-op action.
         *
         * This variable is used in cases such as default initialization,
         * fallback mechanisms, or preventing null reference errors during action lookup.
         */
        Action _emptyAction = {};
        std::unordered_map<std::string, Action> _actions;

        std::vector<sf::Keyboard::Key> _currentKeys;
        std::vector<sf::Mouse::Button> _currentMouseButtons;
        sf::Vector2i _currentMousePosition;
        ModifierKey _currentModifiers;

        void ApplyKeyboardModifiers(Action& action, const std::vector<sf::Keyboard::Key>& modifierKeys);
    };
}
