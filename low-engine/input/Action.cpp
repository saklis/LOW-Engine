#include "Action.h"

namespace LowEngine::Input {
    bool Action::IsKeyPressed(const std::vector<sf::Keyboard::Key>& keys) const {
        return std::find(keys.begin(), keys.end(), Key) != keys.end();
    }

    bool Action::IsMouseButtonPressed(const std::vector<sf::Mouse::Button>& mouseButtons) const {
        return std::find(mouseButtons.begin(), mouseButtons.end(), MouseButton) != mouseButtons.end();
    }
}
