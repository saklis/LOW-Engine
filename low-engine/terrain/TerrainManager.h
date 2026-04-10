#pragma once

#include <vector>

#include "graphics/Sprite.h"

#include "TileMapLayer.h"

namespace LowEngine::Terrain {

	class TerrainManager {
	public:
		void Update(float deltaTime);

		/**
		 * @brief Collect all sprites that need to be drawn for this terrain.
		 *
		 * Sprites will be added to collection passed as parameter.
		 * @param[out] sprites Reference to collection that will be filled with Sprites that needs to be drawn.
		 */
		void CollectSprites(std::vector<Sprite>& sprites);

		void AddEmptyLayer();

		std::vector<LowEngine::TileMap::TileMapLayer>* GetLayers() {
			return &_layers;
		}

		void DeleteLayer(int layerIndex);

		nlohmann::ordered_json SerializeToJSON();

		bool DeserializeFromJSON(const nlohmann::ordered_json& json);

		void CopyLayersFrom(const TerrainManager& terrain);

	protected:
		std::vector<LowEngine::TileMap::TileMapLayer> _layers;
	};
}
