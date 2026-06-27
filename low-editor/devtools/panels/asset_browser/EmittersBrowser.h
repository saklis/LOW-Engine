#pragma once

#include <imgui.h>
#include <string>

namespace LowEngine {
    class Game;

    namespace Panels {
        void DisplayEmitterBrowser(Game& game, ImVec2 size);

        /**
         * @brief Drops the cached thumbnail for the given emitter alias.
         *
         * Call this after writing a new thumbnail to disk so the browser
         * reloads it on the next frame.
         */
        void InvalidateEmitterThumbnail(const std::string& alias);
    }
}