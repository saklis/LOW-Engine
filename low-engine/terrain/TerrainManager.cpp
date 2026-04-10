#include "TerrainManager.h"

namespace LowEngine::Terrain {
    void TerrainManager::Update(float deltaTime) {
        for (auto& layer: _layers) {
            if (Assets::HasSpriteSheet(layer.GetTextureId())) {
                layer.Update(deltaTime, Assets::GetSpriteSheet(layer.GetTextureId()));
            }
        }
    }

    void TerrainManager::CollectSprites(std::vector<Sprite>& sprites) {
        for (auto& layer: _layers) {
            layer.CollectSprites(sprites);
        }
    }

    void TerrainManager::AddEmptyLayer() {
        _layers.emplace_back();
    }

    void TerrainManager::DeleteLayer(int layerIndex) {
        _layers.erase(_layers.begin() + layerIndex);
    }

    bool TerrainManager::DeserializeFromJSON(const nlohmann::ordered_json& json) {
        if (json.contains("layers")) {
            for (auto& layerJson : json["layers"]) {
                _layers.emplace_back();
                if (!_layers.back().DeserializeFromJSON(layerJson)) {
                    return false;
                }
            }
        }
        return true;
    }

    void TerrainManager::CopyLayersFrom(const TerrainManager& terrain) {
        for (auto & layer : terrain._layers) {
            _layers.emplace_back(layer);
        }
    }

    nlohmann::ordered_json TerrainManager::SerializeToJSON() {
        nlohmann::ordered_json json;
        nlohmann::ordered_json layersJson = nlohmann::ordered_json::array();

        for (auto& layer : _layers) {
            layersJson.emplace_back(layer.SerializeToJSON());
        }

        json["layers"] = layersJson;
        return json;
    }
}
