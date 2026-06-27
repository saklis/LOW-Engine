#include "TileMapLayer.h"

namespace LowEngine::TileMap {
	TileMapLayer::TileMapLayer(const TileMapLayer& other)
		: Id(other.Id), Name(other.Name), IsVisible(other.IsVisible),
		  ContributesToNavigation(other.ContributesToNavigation),
		  ContributesToCollision(other.ContributesToCollision),
		  _drawOrder(other._drawOrder), TileSize(other.TileSize),
		  _textureId(other._textureId), _tiles(other._tiles),
		  _staticVertices(other._staticVertices), _staticVertexIndex(other._staticVertexIndex),
		  _animVertices(other._animVertices), _animVertexIndex(other._animVertexIndex) {
	}

	TileMapLayer& TileMapLayer::operator=(const TileMapLayer& other) {
		if (this != &other) {
			Id = other.Id;
			Name = other.Name;
			IsVisible = other.IsVisible;
			ContributesToNavigation = other.ContributesToNavigation;
			ContributesToCollision = other.ContributesToCollision;
			_drawOrder = other._drawOrder;
			TileSize = other.TileSize;
			_textureId = other._textureId;
			_tiles = other._tiles;
			_staticVertices = other._staticVertices;
			_staticVertexIndex = other._staticVertexIndex;
			_animVertices = other._animVertices;
			_animVertexIndex = other._animVertexIndex;
		}
		return *this;
	}

	TileMapLayer::TileMapLayer(TileMapLayer&& other) noexcept
		: Id(std::move(other.Id)), Name(std::move(other.Name)), IsVisible(other.IsVisible),
		  ContributesToNavigation(other.ContributesToNavigation), ContributesToCollision(other.ContributesToCollision),
		  _drawOrder(other._drawOrder), TileSize(other.TileSize),
		  _textureId(other._textureId), _tiles(std::move(other._tiles)),
		  _staticVertices(std::move(other._staticVertices)), _staticVertexIndex(std::move(other._staticVertexIndex)),
		  _animVertices(std::move(other._animVertices)), _animVertexIndex(std::move(other._animVertexIndex)) {
	}

	TileMapLayer& TileMapLayer::operator=(TileMapLayer&& other) noexcept {
		if (this != &other) {
			Id = std::move(other.Id);
			Name = std::move(other.Name);
			IsVisible = other.IsVisible;
			ContributesToNavigation = other.ContributesToNavigation;
			ContributesToCollision = other.ContributesToCollision;
			_drawOrder = other._drawOrder;
			TileSize = other.TileSize;
			_textureId = other._textureId;
			_tiles = std::move(other._tiles);
			_staticVertices = std::move(other._staticVertices);
			_staticVertexIndex = std::move(other._staticVertexIndex);
			_animVertices = std::move(other._animVertices);
			_animVertexIndex = std::move(other._animVertexIndex);
		}
		return *this;
	}

	const Tile* TileMapLayer::FindTile(sf::Vector2i cellCoords) const {
		auto it = _tiles.find(cellCoords);
		return it == _tiles.end() ? nullptr : &it->second;
	}

	Tile* TileMapLayer::FindTile(sf::Vector2i cellCoords) {
		auto it = _tiles.find(cellCoords);
		return it == _tiles.end() ? nullptr : &it->second;
	}

	bool TileMapLayer::DeleteTile(sf::Vector2i cellCoords) {
		const auto erased = _tiles.erase(cellCoords);
		if (erased == 0) {
			return false;
		}

		RebuildStaticVertices();
		RebuildAnimVertices();
		return true;
	}

	void TileMapLayer::SetTextureId(std::size_t textureId) {
		_textureId = textureId;
		RebuildStaticVertices();
		RebuildAnimVertices();
	}

	std::size_t TileMapLayer::GetTextureId() {
		return _textureId;
	}

	int TileMapLayer::GetDrawOrder() {
		return _drawOrder;
	}

	void TileMapLayer::SetDrawOrder(int drawOrder) {
		_drawOrder = drawOrder;
	}

	void TileMapLayer::AddTile(sf::Vector2i cellCoords, sf::IntRect spritesheetCoords, bool skipRebuild) {
		_tiles[cellCoords].Type = TileType::Static;
		_tiles[cellCoords].SpriteRect = spritesheetCoords;
		if (!skipRebuild) RebuildStaticVertices();
	}

	void TileMapLayer::AddTile(sf::Vector2i cellCoords, std::string& animClipName) {
		_tiles[cellCoords].Type = TileType::Animated;
		_tiles[cellCoords].AnimationClipName = animClipName;
		RebuildAnimVertices();
	}

	void TileMapLayer::CollectDrawables(std::vector<SceneDrawable>& drawables) {
		if (!IsVisible || _tiles.empty()) return;

		const sf::Texture& texture = Assets::GetTexture(_textureId);

		if (_staticVertices.getVertexCount() > 0) {
			drawables.emplace_back(VertexArrayDrawable{
				&_staticVertices,
				&texture,
				_drawOrder
			});
		}

		if (_animVertices.getVertexCount() > 0) {
			drawables.emplace_back(VertexArrayDrawable{
				&_animVertices,
				&texture,
				_drawOrder
			});
		}
	}

	void TileMapLayer::Update(float deltaTime, Animation::SpriteSheet& spriteSheet) {
		for (auto& [coords, tile] : _tiles) {
			if (tile.Type != TileType::Animated) continue;
			if (!spriteSheet.HasAnimationClip(tile.AnimationClipName)) continue;

			auto& clip = spriteSheet.GetAnimationClip(tile.AnimationClipName);
			tile.AnimSpriteTimer += deltaTime;
			if (tile.AnimSpriteTimer >= clip.FrameDuration) {
				tile.AnimSpriteTimer -= clip.FrameDuration;
				tile.AnimSpriteCurrentFrame = (tile.AnimSpriteCurrentFrame + 1) % clip.FrameCount;
				tile.SpriteRect = clip.Frames[tile.AnimSpriteCurrentFrame];

				auto it = _animVertexIndex.find(coords);
				if (it != _animVertexIndex.end()) {
					UpdateAnimVertexUVs(it->second, tile.SpriteRect);
				}
			}
		}
	}

	bool TileMapLayer::DeserializeFromJSON(const nlohmann::ordered_json& json) {
		if (json.contains("id")) {
			Id = json["id"].get<std::string>();
		}
		if (json.contains("name")) {
			Name = json["name"].get<std::string>();
		}
		if (json.contains("isVisible")) {
			IsVisible = json["isVisible"].get<bool>();
		}
		if (json.contains("contributesToNavigation")) {
			ContributesToNavigation = json["contributesToNavigation"].get<bool>();
		}
		if (json.contains("contributesToCollision")) {
			ContributesToCollision = json["contributesToCollision"].get<bool>();
		}
		if (json.contains("tileSize")) {
			TileSize.x = json["tileSize"]["x"].get<std::size_t>();
			TileSize.y = json["tileSize"]["y"].get<std::size_t>();
		}
		if (json.contains("textureAlias")) {
			SetTextureId(Assets::GetTextureId(json["textureAlias"].get<std::string>()));
		} else if (json.contains("textureId")) {
			// legacy: fallback to raw numeric ID (may assign wrong texture if load order changed)
			SetTextureId(json["textureId"].get<std::size_t>());
		}
		if (json.contains("drawOrder")) {
			SetDrawOrder(json["drawOrder"].get<int>());
		}
		if (json.contains("tiles")) {
			for (auto& tileJson : json["tiles"]) {
				sf::Vector2i coords = {tileJson["cellX"].get<int>(), tileJson["cellY"].get<int>()};
				auto type = static_cast<TileType>(tileJson["type"].get<std::uint8_t>());
				if (type == TileType::Animated) {
					auto clipName = tileJson["animClipName"].get<std::string>();
					AddTile(coords, clipName);
				} else {
					sf::IntRect rect(
						{tileJson["spriteRect"]["x"].get<int>(), tileJson["spriteRect"]["y"].get<int>()},
						{tileJson["spriteRect"]["w"].get<int>(), tileJson["spriteRect"]["h"].get<int>()}
					);
					AddTile(coords, rect, true);
				}
				auto& tile = _tiles[coords];
				if (tileJson.contains("hasCollision")) {
					tile.HasCollision = tileJson["hasCollision"].get<bool>();
				}
				if (tileJson.contains("traversalMask")) {
					tile.TraversalMask = tileJson["traversalMask"].get<std::uint8_t>();
				}
				if (tileJson.contains("entryCost")) {
					tile.EntryCost = tileJson["entryCost"].get<std::uint8_t>();
				}
			}
		}
		RebuildStaticVertices();
		return true;
	}

	nlohmann::ordered_json TileMapLayer::SerializeToJSON() {
		nlohmann::ordered_json json;
		json["id"] = Id;
		json["name"] = Name;
		json["isVisible"] = IsVisible;
		json["contributesToNavigation"] = ContributesToNavigation;
		json["contributesToCollision"] = ContributesToCollision;
		json["tileSize"] = {{"x", TileSize.x}, {"y", TileSize.y}};
		json["drawOrder"] = _drawOrder;
		json["textureAlias"] = Assets::GetTextureAlias(_textureId);

		nlohmann::ordered_json tilesJson = nlohmann::ordered_json::array();
		for (auto& [coords, tile] : _tiles) {
			nlohmann::ordered_json tileJson;
			tileJson["cellX"] = coords.x;
			tileJson["cellY"] = coords.y;
			tileJson["type"] = static_cast<std::uint8_t>(tile.Type);
			tileJson["spriteRect"] = {
				{"x", tile.SpriteRect.position.x},
				{"y", tile.SpriteRect.position.y},
				{"w", tile.SpriteRect.size.x},
				{"h", tile.SpriteRect.size.y}
			};
			tileJson["animClipName"] = tile.AnimationClipName;
			tileJson["hasCollision"] = tile.HasCollision;
			tileJson["traversalMask"] = tile.TraversalMask;
			tileJson["entryCost"] = tile.EntryCost;
			tilesJson.emplace_back(tileJson);
		}
		json["tiles"] = tilesJson;

		return json;
	}

	void TileMapLayer::RebuildStaticVertices() {
		_staticVertices.clear();
		_staticVertexIndex.clear();

		std::size_t idx = 0;
		for (auto& [coords, tile] : _tiles) {
			if (tile.Type != TileType::Static) continue;

			_staticVertexIndex[coords] = idx;
			_staticVertices.resize(idx + 6);

			float x = static_cast<float>(coords.x) * static_cast<float>(TileSize.x);
			float y = static_cast<float>(coords.y) * static_cast<float>(TileSize.y);
			float w = static_cast<float>(TileSize.x);
			float h = static_cast<float>(TileSize.y);

			_staticVertices[idx + 0].position = {x, y};
			_staticVertices[idx + 1].position = {x + w, y};
			_staticVertices[idx + 2].position = {x, y + h};
			_staticVertices[idx + 3].position = {x + w, y};
			_staticVertices[idx + 4].position = {x + w, y + h};
			_staticVertices[idx + 5].position = {x, y + h};

			float u0 = static_cast<float>(tile.SpriteRect.position.x);
			float v0 = static_cast<float>(tile.SpriteRect.position.y);
			float u1 = u0 + static_cast<float>(tile.SpriteRect.size.x);
			float v1 = v0 + static_cast<float>(tile.SpriteRect.size.y);

			_staticVertices[idx + 0].texCoords = {u0, v0};
			_staticVertices[idx + 1].texCoords = {u1, v0};
			_staticVertices[idx + 2].texCoords = {u0, v1};
			_staticVertices[idx + 3].texCoords = {u1, v0};
			_staticVertices[idx + 4].texCoords = {u1, v1};
			_staticVertices[idx + 5].texCoords = {u0, v1};

			idx += 6;
		}
	}

	void TileMapLayer::RebuildAnimVertices() {
		_animVertices.clear();
		_animVertexIndex.clear();

		std::size_t idx = 0;
		for (auto& [coords, tile] : _tiles) {
			if (tile.Type != TileType::Animated) continue;

			_animVertexIndex[coords] = idx;
			_animVertices.resize(idx + 6);

			float x = static_cast<float>(coords.x) * static_cast<float>(TileSize.x);
			float y = static_cast<float>(coords.y) * static_cast<float>(TileSize.y);
			float w = static_cast<float>(TileSize.x);
			float h = static_cast<float>(TileSize.y);

			_animVertices[idx + 0].position = {x, y};
			_animVertices[idx + 1].position = {x + w, y};
			_animVertices[idx + 2].position = {x, y + h};
			_animVertices[idx + 3].position = {x + w, y};
			_animVertices[idx + 4].position = {x + w, y + h};
			_animVertices[idx + 5].position = {x, y + h};

			UpdateAnimVertexUVs(idx, tile.SpriteRect);
			idx += 6;
		}
	}

	void TileMapLayer::UpdateAnimVertexUVs(std::size_t idx, const sf::IntRect& rect) {
		float u0 = static_cast<float>(rect.position.x);
		float v0 = static_cast<float>(rect.position.y);
		float u1 = u0 + static_cast<float>(rect.size.x);
		float v1 = v0 + static_cast<float>(rect.size.y);

		_animVertices[idx + 0].texCoords = {u0, v0};
		_animVertices[idx + 1].texCoords = {u1, v0};
		_animVertices[idx + 2].texCoords = {u0, v1};
		_animVertices[idx + 3].texCoords = {u1, v0};
		_animVertices[idx + 4].texCoords = {u1, v1};
		_animVertices[idx + 5].texCoords = {u0, v1};
	}
}
