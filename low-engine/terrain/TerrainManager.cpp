#include "TerrainManager.h"

#include "box2d/box2d.h"
#include "box2d/types.h"

namespace LowEngine::Terrain {
    void TerrainManager::Update(float deltaTime) {
        for (auto& layer: _layers) {
            if (Assets::HasSpriteSheet(layer.GetTextureId())) {
                layer.Update(deltaTime, Assets::GetSpriteSheet(layer.GetTextureId()));
            }
        }
    }

    void TerrainManager::CollectDrawables(std::vector<SceneDrawable>& drawables) {
        for (auto& layer: _layers) {
            layer.CollectDrawables(drawables);
        }
    }

    void TerrainManager::AddEmptyLayer() {
        _layers.emplace_back();
        _navigationDirty = true;
        _collisionsDirty = true;
    }

    void TerrainManager::DeleteLayer(int layerIndex) {
        _layers.erase(_layers.begin() + layerIndex);
        _navigationDirty = true;
        _collisionsDirty = true;
    }

    bool TerrainManager::DeserializeFromJSON(const nlohmann::ordered_json& json) {
        if (json.contains("navBounds")) {
            auto& b = json["navBounds"];
            NavBounds = sf::IntRect(
                {b["x"].get<int>(), b["y"].get<int>()},
                {b["w"].get<int>(), b["h"].get<int>()}
            );
        }
        if (json.contains("layers")) {
            for (auto& layerJson: json["layers"]) {
                _layers.emplace_back();
                if (!_layers.back().DeserializeFromJSON(layerJson)) {
                    return false;
                }
            }
        }
        _navigationDirty = true;
        _collisionsDirty = true;
        return true;
    }

    void TerrainManager::CopyLayersFrom(const TerrainManager& terrain) {
        for (auto& layer: terrain._layers) {
            _layers.emplace_back(layer);
        }
        NavBounds = terrain.NavBounds;
        _navigationDirty = true;
        _collisionsDirty = true;
    }

    nlohmann::ordered_json TerrainManager::SerializeToJSON() {
        nlohmann::ordered_json json;
        json["navBounds"] = {
            {"x", NavBounds.position.x},
            {"y", NavBounds.position.y},
            {"w", NavBounds.size.x},
            {"h", NavBounds.size.y}
        };

        nlohmann::ordered_json layersJson = nlohmann::ordered_json::array();
        for (auto& layer: _layers) {
            layersJson.emplace_back(layer.SerializeToJSON());
        }
        json["layers"] = layersJson;
        return json;
    }

    Navigation::NavigationGrid& TerrainManager::GetNavGrid() {
        if (_navigationDirty) BakeNavGrid();
        return _navGrid;
    }

    void TerrainManager::BakeNavGrid() {
        const auto size = NavBounds.size;
        if (size.x <= 0 || size.y <= 0) {
            _navGrid.Cells.clear();
            _navGrid.Width = 0;
            _navGrid.Height = 0;
            _navigationDirty = false;
            return;
        }

        _navGrid.Width = static_cast<std::size_t>(size.x);
        _navGrid.Height = static_cast<std::size_t>(size.y);
        _navGrid.Cells.assign(_navGrid.Width * _navGrid.Height, Navigation::NavigationCell{});

        for (int localY = 0; localY < size.y; ++localY) {
            for (int localX = 0; localX < size.x; ++localX) {
                const sf::Vector2i worldCell{
                    NavBounds.position.x + localX,
                    NavBounds.position.y + localY
                };
                const std::size_t idx =
                        static_cast<std::size_t>(localX) +
                        static_cast<std::size_t>(localY) * _navGrid.Width;

                auto& cell = _navGrid.Cells[idx];
                cell.Position = {
                    static_cast<unsigned>(localX),
                    static_cast<unsigned>(localY)
                };

                std::uint8_t mask = ::LowEngine::TileMap::Traversal::All;
                std::uint8_t cost = 1;
                bool anyContribution = false;

                for (const auto& layer: _layers) {
                    if (!layer.ContributesToNavigation) continue;
                    const ::LowEngine::TileMap::Tile* tile = layer.FindTile(worldCell);
                    if (!tile) continue;
                    anyContribution = true;
                    mask &= tile->TraversalMask;
                    if (tile->EntryCost > cost) cost = tile->EntryCost;
                }

                if (!anyContribution) {
                    mask = ::LowEngine::TileMap::Traversal::None;
                }

                cell.IsWalkable = (mask & ::LowEngine::TileMap::Traversal::Walk) != 0;
                cell.IsSwimmable = (mask & ::LowEngine::TileMap::Traversal::Swim) != 0;
                cell.IsFlyable = (mask & ::LowEngine::TileMap::Traversal::Fly) != 0;
                cell.MoveCost = static_cast<float>(cost);
            }
        }

        _navigationDirty = false;
    }

    void TerrainManager::BakeCollisions(b2WorldId worldId) {
        if (!_collisionsDirty) return;
        if (b2World_IsValid(worldId) == false) {
            _log->error("Invalid world ID provided for collision baking");
            return;
        }

        ClearCollisions();

        for (const auto& layer: _layers) {
            if (!layer.ContributesToCollision) continue;
            if (layer.TileSize.x == 0 || layer.TileSize.y == 0) continue;

            auto& tiles = layer.GetTiles();
            for (auto& [cell, tile]: tiles) {
                if (tile.HasCollision == false) continue;

                if (!CreateBodyIfEmpty(worldId)) return;

                float x = static_cast<float>(cell.x) * layer.TileSize.x;
                float y = static_cast<float>(cell.y) * layer.TileSize.y;
                float centerX = x + static_cast<float>(layer.TileSize.x) * 0.5f;
                float centerY = y + static_cast<float>(layer.TileSize.y) * 0.5f;

                auto box = b2MakeOffsetBox(layer.TileSize.x * 0.5f, layer.TileSize.y * 0.5f,
                                           {centerX, centerY}, b2MakeRot(0.0f));
                b2ShapeDef shapeDef = b2DefaultShapeDef();
                shapeDef.isSensor = false;
                shapeDef.enableContactEvents = true;
                b2CreatePolygonShape(_collisionBodyId, &shapeDef, &box);
            }
        }

        _collisionsDirty = false;
    }

    void TerrainManager::ClearCollisions() {
        if (B2_IS_NON_NULL(_collisionBodyId) && b2Body_IsValid(_collisionBodyId)) {
            b2DestroyBody(_collisionBodyId);
        }
        _collisionBodyId = b2_nullBodyId;
        _collisionsDirty = true;
    }

    bool TerrainManager::CreateBodyIfEmpty(b2WorldId worldId) {
        if (B2_IS_NULL(_collisionBodyId)) {
            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = b2_staticBody;
            bodyDef.position = {0.0f, 0.0f};
            _collisionBodyId = b2CreateBody(worldId, &bodyDef);
            if (B2_IS_NULL(_collisionBodyId) || !b2Body_IsValid(_collisionBodyId)) {
                _log->error("Failed to create terrain collision body");
                return false;
            }
        }

        return true;
    }
}
