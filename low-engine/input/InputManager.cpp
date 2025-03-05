
#include "InputManager.h"

void LowEngine::Input::InputManager::AddAction(const std::string& actionName, sf::Keyboard::Key key, bool shift, bool ctrl, bool alt) {
    Action newAction;
    newAction.Name = actionName;
    newAction.Key = key;
    newAction.Shift = shift;
    newAction.Ctrl = ctrl;
    newAction.Alt = alt;

    _actions[actionName] = std::move(newAction);
}

LowEngine::Input::Action& LowEngine::Input::InputManager::GetAction(const std::string& actionName) {
    auto action = _actions.find(actionName);
    if (action != _actions.end()) {
        return action->second;
    }

    static Action emptyAction;
    return emptyAction;
}
