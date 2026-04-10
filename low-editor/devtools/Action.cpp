#include "Action.h"

namespace LowEngine {
    Mouse* EditorAction::Action(LowEngine::MouseAction action) {
        return &GetInstance()->_mouseActions.at(action);
    }

    Mouse* EditorAction::Action(MouseScrollAction action) {
        return &GetInstance()->_mouseScrollActions.at(action);
    }

    void EditorAction::ClearActions() {
        for (auto& mouseAction: GetInstance()->_mouseActions) {
            mouseAction.second.Started = false;
            mouseAction.second.Stopped = false;
        }

        for (auto& mouseScrollAction : GetInstance()->_mouseScrollActions) {
            mouseScrollAction.second.Started = false;
            mouseScrollAction.second.Stopped = false;
        }
    }

    EditorAction::EditorAction() {
        _mouseActions[MouseAction::Primary] = Mouse();
        _mouseActions[MouseAction::Secondary] = Mouse();
        _mouseActions[MouseAction::Tertiary] = Mouse();

        _mouseScrollActions[MouseScrollAction::Up] = Mouse();
        _mouseScrollActions[MouseScrollAction::Down] = Mouse();
    }
}
