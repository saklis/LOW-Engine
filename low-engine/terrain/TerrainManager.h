#pragma once

#include <vector>

#include "SFML/Graphics/Rect.hpp"

#include "graphics/Sprite.h"

#include "TileMapLayer.h"
#include "box2d/id.h"
#include "navigation/NavigationGrid.h"

namespace LowEngine::Terrain {

	class TerrainManager {
	public:
		/**
		 * @brief Rectangular playable area, in tile-cell coordinates.
		 *
		 * Defines the bounds of the baked NavigationGrid. Tiles placed outside
		 * this rectangle are rendered but ignored by pathfinding. position is
		 * the min corner (inclusive), size is the extent in cells.
		 */
		sf::IntRect NavBounds = sf::IntRect({0, 0}, {0, 0});

		void Update(float deltaTime);

		/**
		 * @brief Collect all drawables that need to be drawn for this terrain.
		 *
		 * Drawables will be added to collection passed as parameter.
		 * @param[out] drawables Reference to collection that will be filled with drawables that need to be drawn.
		 */
		void CollectDrawables(std::vector<SceneDrawable>& drawables);

		void AddEmptyLayer();

		std::vector<LowEngine::TileMap::TileMapLayer>* GetLayers() {
			return &_layers;
		}

		[[nodiscard]] const std::vector<LowEngine::TileMap::TileMapLayer>* GetLayers() const {
			return &_layers;
		}

		void DeleteLayer(int layerIndex);

		nlohmann::ordered_json SerializeToJSON();

		bool DeserializeFromJSON(const nlohmann::ordered_json& json);

		void CopyLayersFrom(const TerrainManager& terrain);

		/**
		 * @brief Get the navigation grid, baking it first if dirty.
		 *
		 * Callers (pathfinder, AI) should use this accessor rather than
		 * touching the grid directly so lazy rebakes stay transparent.
		 */
		Navigation::NavigationGrid& GetNavGrid();

		/**
		 * @brief Flag the navigation grid for rebake on next GetNavGrid() call.
		 *
		 * Call after any edit that could change traversability: tile add/remove,
		 * layer ContributesToNav toggle, NavBounds resize, or direct
		 * TraversalMask/EntryCost edits on tiles.
		 */
		void MarkNavigationDirty() { _navigationDirty = true; }

		void MarkCollisionsDirty() { _collisionsDirty = true; }

		/**
		 * @brief Rebuild the navigation grid from the current layer stack.
		 *
		 * For each cell in NavBounds: starts from Traversal::All, ANDs the
		 * TraversalMask of every tile found on a ContributesToNav layer, and
		 * takes the max EntryCost across those tiles. Cells with no contributing
		 * tile are fully blocked.
		 */
		void BakeNavGrid();

		void BakeCollisions(b2WorldId worldId);
		void ClearCollisions();

	protected:
		std::vector<LowEngine::TileMap::TileMapLayer> _layers;

		Navigation::NavigationGrid _navGrid;
		bool _navigationDirty = true;

		b2BodyId _collisionBodyId = b2_nullBodyId;
		bool _collisionsDirty = true;

		bool CreateBodyIfEmpty(b2WorldId worldId);
	};
}
