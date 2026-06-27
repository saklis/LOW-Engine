#pragma once

#include <cstddef>
#include <cstdint>

#include <SFML/System/Vector2.hpp>

#include "SFML/Graphics/Rect.hpp"
#include "terrain/Tile.h"

namespace sf {
	class RenderWindow;
}

namespace LowEngine {
	namespace Tools {
		struct TerrainBrush;
		class TerrainBrushManager;
	}

	class Game;
	class Scene;

	namespace Panels {
		void DrawTerrainToolbar(const sf::Vector2u& displaySize, bool& isInTerrainEditMode);

		void DrawTerrainProperties(Scene* scene, int posX, int posY, int width, int height,
		                           size_t& selectedLayerIndex, int resetLayoutFrames);

		void DrawTerrainNavigationEditor(Scene* scene, int posX, int posY, int width, int height,
		                                 bool& navOverlayVisible, int resetLayoutFrames);
		void DrawTerrainBrushManager(Scene* scene, int posX, int posY, int width, int height,
		                             size_t& selectedLayerIndex, Tools::TerrainBrushManager& brushManager,
		                             sf::IntRect& selectedTerrainLayerTile,
		                             TileMap::TileType& selectedTileType, std::string& selectedAnimClipName,
		                             std::uint8_t& brushTraversalMask, std::uint8_t& brushEntryCost,
		                             bool& brushHasCollision,
		                             size_t& selectedBrushIndex, int resetLayoutFrames);

		void DrawTerrainLayerEditor(Scene* scene, int posX, int posY, int width, int height,
		                            size_t& selectedLayerIndex, sf::IntRect& selectedTerrainLayerTile,
		                            TileMap::TileType& selectedTileType, std::string& selectedAnimClipName,
		                            std::uint8_t& brushTraversalMask, std::uint8_t& brushEntryCost, int resetLayoutFrames);

		void Terrain2GameSceneInteraction(Game& game, Scene* scene, size_t selectedLayerIndex,
		                                  sf::IntRect selectedTerrainLayerTile, TileMap::TileType& selectedTileType,
		                                  std::string& selectedAnimClipName,
		                                  std::uint8_t brushTraversalMask, std::uint8_t brushEntryCost, bool brushHasCollision);
		
		void DrawTerrainWorldGrid(sf::RenderWindow& window, Scene* scene, size_t selectedLayerIndex);

		void DrawNavOverlay(sf::RenderWindow& window, Scene* scene, size_t selectedLayerIndex);

		void ApplyBrushAsCurrentPaint(const Tools::TerrainBrush& brush, sf::IntRect& selectedTerrainLayerTile,
		                              TileMap::TileType& selectedTileType, std::string& selectedAnimClipName,
		                              std::uint8_t& brushTraversalMask, std::uint8_t& brushEntryCost,
		                              bool& brushHasCollision);
	}
}
