#pragma once

#include <string>
#include <nlohmann/json_fwd.hpp>

#include "terrain/Tile.h"

namespace LowEngine::Tools {
	struct TerrainBrush {
		std::string Name = "Brush";
		TileMap::TileType Type = TileMap::TileType::Static;
		sf::IntRect SpriteRect = sf::IntRect();
		std::string AnimationClipName;
		std::uint8_t TraversalMask = TileMap::Traversal::None;
		std::uint8_t EntryCost = 1;
		bool HasCollision = false;

		[[nodiscard]] nlohmann::ordered_json SerializeToJSON();
		bool DeserializeFromJSON(const nlohmann::ordered_json& json);
	};
}
