#pragma once

#include <string>

namespace LowEngine {
    class Game;

    namespace Panels {
        void DrawInputEditor(Game& game, bool& isVisible, std::string& actionBeingBound);
    }
}