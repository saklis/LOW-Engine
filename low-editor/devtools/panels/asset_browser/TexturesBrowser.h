#pragma once

#include <imgui.h>

namespace LowEngine {
    class Game;

    namespace Panels {
        void DisplayTextureBrowser(Game& game, ImVec2 size);
    }
}