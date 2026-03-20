#pragma once

#include <cstddef>

#include <SFML/System/Vector2.hpp>

namespace LowEngine {
    class Scene;

    namespace Panels {
        void DrawTerrainToolbar(const sf::Vector2u& displaySize, bool& isInTerrainEditMode);

        void DrawTerrainLayersList(Scene* scene, int posX, int posY, int width, int height,
                                   size_t& selectedLayerIndex, int resetLayoutFrames);

        void DrawTerrainLayerEditor(Scene* scene, int posX, int posY, int width, int height,
                                    size_t& selectedLayerIndex, int resetLayoutFrames);
    }
}