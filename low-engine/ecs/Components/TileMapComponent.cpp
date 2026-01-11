#include "TileMapComponent.h"

namespace LowEngine::ECS {
	void TileMapComponent::Update(float deltaTime) {
		auto& map = Assets::GetTileMap(_mapId);
		map.Update(deltaTime);

		auto transformComponent = _memory->GetComponent<TransformComponent>(EntityId);
		_sprite.setPosition(transformComponent->Position);
		_sprite.setRotation(transformComponent->Rotation);
		_sprite.setScale(transformComponent->Scale);
		_sprite.Layer = Layer;
	}

	Sprite* TileMapComponent::Draw() {
		auto& map = Assets::GetTileMap(_mapId);

		_texture.clear(sf::Color::Magenta);

		auto terrain = map.TerrainLayer.GetDrawable();
		if (terrain) { _texture.draw(*terrain); }
		auto features = map.FeaturesLayer.GetDrawable();
		if (features) { _texture.draw(*features); }

		_texture.display();

		_sprite.setTexture(_texture.getTexture());
		return &_sprite;
	}

	nlohmann::ordered_json TileMapComponent::SerializeToJSON() {
		nlohmann::ordered_json json = IComponent::SerializeToJSON();
		json["MapId"] = _mapId;
		json["Layer"] = Layer;
		return json;
	}

	bool TileMapComponent::DeserializeFromJSON(const nlohmann::ordered_json& jsonData) {
		if (!IComponent::DeserializeFromJSON(jsonData)) {
			_log->error("TileMapComponent deserialization failed: base component data not set.");
			return false;
		}
		if (jsonData.contains("MapId")) {
			_mapId = jsonData["MapId"].get<size_t>();
			auto& map = Assets::GetTileMap(_mapId);
			Resize(map);
		} else {
			_log->error("TileMapComponent deserialization failed: missing 'MapId' field.");
			return false;
		}
		if (jsonData.contains("Layer")) {
			Layer = jsonData["Layer"].get<int>();
		} else {
			_log->error("TileMapComponent deserialization failed: missing 'Layer' field.");
			return false;
		}
		
		return true;
	}

	void TileMapComponent::SetMapId(size_t mapId) {
		auto& map = Assets::GetTileMap(mapId);

		_mapId = mapId;
		Resize(map);
	}

	std::vector<sf::Vector2f> TileMapComponent::FindPath(sf::Vector2f start, sf::Vector2f end,
	                                                     Terrain::Navigation::MovementType movementType) {
		auto& map = Assets::GetTileMap(_mapId);

		auto offset = _sprite.getPosition();
		offset.x = -offset.x;
		offset.y = -offset.y;

		auto cellSize = map.TerrainLayer.CellSize;

		// Convert start and end positions to NavGrid coordinates
		sf::Vector2u startCell = {
			static_cast<unsigned>((start.x + offset.x) / cellSize),
			static_cast<unsigned>((start.y + offset.y) / cellSize)
		};
		sf::Vector2u endCell = {
			static_cast<unsigned>((end.x + offset.x) / cellSize),
			static_cast<unsigned>((end.y + offset.y) / cellSize)
		};

		if (startCell.x >= map.NavGrid.Width || startCell.y >= map.NavGrid.Height ||
			endCell.x >= map.NavGrid.Width || endCell.y >= map.NavGrid.Height) {
			//_log->warn("Tile Map -> FindPath: Start or end position is out of bounds of the navigation grid.");
			return {};
		}

		auto navPath = map.NavGrid.FindPath(startCell, endCell, movementType);

		// return as vector of sf::Vector2f points
		if (!navPath.empty()) {
			std::vector<sf::Vector2f> result;
			for (auto& cell : navPath) {
				sf::Vector2f point = {
					static_cast<float>(cell.Position.x) * cellSize + offset.x,
					static_cast<float>(cell.Position.y) * cellSize + offset.y
				};
				result.emplace_back(point);
			}

			return result;
		}

		return {};
	}

	void TileMapComponent::Resize(Terrain::TileMap& map) {
		if (!_texture.resize({static_cast<unsigned>(map.Size.x), static_cast<unsigned>(map.Size.y)})) {
			_log->error("Failed to resize map render texture to {}x{}.", map.Size.x, map.Size.y);
		}
		_texture.clear();

		_sprite.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(map.Size.x), static_cast<int>(map.Size.y)}));
	}
}
