#include "InputManager.h"

namespace LowEngine::Input {
    sf::Vector2i InputManager::GetMousePosition() {
        return _currentMousePosition;
    }

    const Action* InputManager::GetAction(const std::string& actionName) const {
        auto action = _actions.find(actionName);
        if (action != _actions.end()) {
            return &(action->second);
        }

        return nullptr;
    }

    void InputManager::ClearActionState() {
        _inputChanged = false;

        // reset state of actions that started/ended in previous update
        for (auto& action: _actions) {
            action.second.Started = false;
            action.second.Ended = false;
        }
    }

    void InputManager::Read(const std::optional<sf::Event>& event) {
        // current state
        if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
            _currentMousePosition = mouseMoved->position;
        }
        if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            _currentMouseButtons.emplace_back(mouseButtonPressed->button);
            _inputChanged = true;
        }
        if (const auto* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
            auto it = std::find(_currentMouseButtons.begin(), _currentMouseButtons.end(), mouseButtonReleased->button);
            if (it != _currentMouseButtons.end()) {
                _currentMouseButtons.erase(it);
            }
            _inputChanged = true;
        }
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            // read modifiers
            if (keyPressed->code == sf::Keyboard::Key::LShift) {
                _currentModifiers.LShift = true;
                _inputChanged = true;
            } else if (keyPressed->code == sf::Keyboard::Key::LControl) {
                _currentModifiers.LCtrl = true;
                _inputChanged = true;
            } else if (keyPressed->code == sf::Keyboard::Key::LAlt) {
                _currentModifiers.LAlt = true;
                _inputChanged = true;
            } else if (keyPressed->code == sf::Keyboard::Key::RShift) {
                _currentModifiers.RShift = true;
                _inputChanged = true;
            } else if (keyPressed->code == sf::Keyboard::Key::RControl) {
                _currentModifiers.RCtrl = true;
                _inputChanged = true;
            } else if (keyPressed->code == sf::Keyboard::Key::RAlt) {
                _currentModifiers.RAlt = true;
                _inputChanged = true;
            } else {
                _currentKeys.emplace_back(keyPressed->code);
                _inputChanged = true;
            }
        }
        if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
            // read modifiers
            if (keyReleased->code == sf::Keyboard::Key::LShift) {
                _currentModifiers.LShift = false;
                _inputChanged = true;
            } else if (keyReleased->code == sf::Keyboard::Key::LControl) {
                _currentModifiers.LCtrl = false;
                _inputChanged = true;
            } else if (keyReleased->code == sf::Keyboard::Key::LAlt) {
                _currentModifiers.LAlt = false;
                _inputChanged = true;
            } else if (keyReleased->code == sf::Keyboard::Key::RShift) {
                _currentModifiers.RShift = false;
                _inputChanged = true;
            } else if (keyReleased->code == sf::Keyboard::Key::RControl) {
                _currentModifiers.RCtrl = false;
                _inputChanged = true;
            } else if (keyReleased->code == sf::Keyboard::Key::RAlt) {
                _currentModifiers.RAlt = false;
                _inputChanged = true;
            } else {
                auto it = std::find(_currentKeys.begin(), _currentKeys.end(), keyReleased->code);
                if (it != _currentKeys.end()) {
                    _currentKeys.erase(it);
                }
                _inputChanged = true;
            }
        }
    }

    void InputManager::Update() {
        if (_inputChanged) {
            for (auto& action: _actions) {
                if (action.second.Type == ActionType::Keyboard) {
                    if (action.second.Active) {
                        if (!action.second.IsKeyPressed(_currentKeys) || action.second != _currentModifiers) {
                            action.second.Active = false;
                            action.second.Ended = true;
                        }
                    } else {
                        if (action.second.IsKeyPressed(_currentKeys) && action.second == _currentModifiers) {
                            action.second.Active = true;
                            action.second.Started = true;
                        }
                    }
                }
                if (action.second.Type == ActionType::Mouse) {
                    if (action.second.Active) {
                        if (!action.second.IsMouseButtonPressed(_currentMouseButtons) || action.second !=
                            _currentModifiers) {
                            action.second.Active = false;
                            action.second.Ended = true;
                        }
                    } else {
                        if (action.second.IsMouseButtonPressed(_currentMouseButtons) && action.second ==
                            _currentModifiers) {
                            action.second.Active = true;
                            action.second.Started = true;
                        }
                    }
                }
            }
        }
    }

    void InputManager::ApplyKeyboardModifiers(Action& action, const std::vector<sf::Keyboard::Key>& modifierKeys) {
        for (const auto& modifier: modifierKeys) {
            switch (modifier) {
                case sf::Keyboard::Key::LShift: action.LShift = true;
                    break;
                case sf::Keyboard::Key::LControl: action.LCtrl = true;
                    break;
                case sf::Keyboard::Key::LAlt: action.LAlt = true;
                    break;
                case sf::Keyboard::Key::RShift: action.RShift = true;
                    break;
                case sf::Keyboard::Key::RControl: action.RCtrl = true;
                    break;
                case sf::Keyboard::Key::RAlt: action.RAlt = true;
                    break;
                default: break;
            }
        }
    }
}
