#include "TerrainBrush.h"

#include <nlohmann/json.hpp>

namespace LowEngine::Tools {
	nlohmann::ordered_json TerrainBrush::SerializeToJSON() {
		nlohmann::ordered_json json;
		json["name"] = Name;
		json["type"] = static_cast<std::uint8_t>(Type);
		json["spriteRect"] = {
			{"x", SpriteRect.position.x},
			{"y", SpriteRect.position.y},
			{"w", SpriteRect.size.x},
			{"h", SpriteRect.size.y}
		};
		json["animClipName"] = AnimationClipName;
		json["traversalMask"] = TraversalMask;
		json["entryCost"] = EntryCost;
		json["hasCollision"] = HasCollision;
		return json;
	}

	bool TerrainBrush::DeserializeFromJSON(const nlohmann::ordered_json& json) {
		if (json.contains("name")) {
			Name = json["name"].get<std::string>();
		}
		if (json.contains("type")) {
			Type = static_cast<TileMap::TileType>(json["type"].get<std::uint8_t>());
		}
		if (json.contains("spriteRect")) {
			SpriteRect = sf::IntRect(
				{json["spriteRect"]["x"].get<int>(), json["spriteRect"]["y"].get<int>()},
				{json["spriteRect"]["w"].get<int>(), json["spriteRect"]["h"].get<int>()}
			);
		}
		if (json.contains("animClipName")) {
			AnimationClipName = json["animClipName"].get<std::string>();
		}
		if (json.contains("traversalMask")) {
			TraversalMask = json["traversalMask"].get<std::uint8_t>();
		}
		if (json.contains("entryCost")) {
			EntryCost = json["entryCost"].get<std::uint8_t>();
		}
		if (json.contains("hasCollision")) {
			HasCollision = json["hasCollision"].get<bool>();
		}

		return true;
	}
}
