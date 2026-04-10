#pragma once

#include <cstddef>

#include <SFML/System/Vector2.hpp>

#include "SFML/Graphics/Rect.hpp"
#include "terrain/Tile.h"

namespace sf {
    class RenderWindow;
}

namespace LowEngine {
    class Game;
    class Scene;

    namespace Panels {
        void DrawTerrainToolbar(const sf::Vector2u& displaySize, bool& isInTerrainEditMode);

        void DrawTerrainLayersList(Scene* scene, int posX, int posY, int width, int height,
                                   size_t& selectedLayerIndex, int resetLayoutFrames);

        void DrawTerrainLayerEditor(Scene* scene, int posX, int posY, int width, int height,
                                    size_t& selectedLayerIndex, sf::IntRect& selectedTerrainLayerTile,
                                    TileMap::TileType& selectedTileType, std::string& selectedAnimClipName, int resetLayoutFrames);

        void Terrain2GameSceneInteraction(Game& game, Scene* scene, size_t selectedLayerIndex,
                                          sf::IntRect selectedTerrainLayerTile, TileMap::TileType& selectedTileType, std::string& selectedAnimClipName);

        void DrawTerrainWorldGrid(sf::RenderWindow& window, Scene* scene, size_t selectedLayerIndex);
    }
}
