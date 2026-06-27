#pragma once

#include <imgui.h>

namespace LowEngine {
    class Game;

    namespace Panels {
        void DisplaySoundBrowser(const Game& game, ImVec2 size);
    }
}