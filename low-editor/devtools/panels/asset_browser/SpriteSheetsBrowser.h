#pragma once

#include <imgui.h>

namespace LowEngine {
    class Game;

    namespace Panels {
        void DisplaySpriteSheetBrowser(Game& game, ImVec2 size);
    }
}