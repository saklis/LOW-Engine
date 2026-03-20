#pragma once

#include <cstddef>
#include <vector>

#include <SFML/System/Vector2.hpp>

namespace LowEngine {
    class Game;
    class Scene;
    class ComponentEditorBinding;

    namespace Panels {
        void DrawLog(size_t posX, size_t posY, size_t sizeX, size_t sizeY, int resetLayoutFrames);

        void DrawToolbar(Game& game, const sf::Vector2u& displaySize, float yOffset, size_t& selectedEntityId);

        void DrawWorldOutliner(Scene* scene, int posX, int posY, int width, int height,
                               size_t& selectedEntityId, int resetLayoutFrames);

        void DrawProperties(Scene* scene, int posX, int posY, int width, int height,
                            size_t& selectedEntityId, int resetLayoutFrames,
                            const std::vector<ComponentEditorBinding>& bindings);
    }
}