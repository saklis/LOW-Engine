#include "TerrainManager.h"

namespace LowEngine::Terrain {
    void TerrainManager::CollectSprites(std::vector<Sprite> &sprites) {
    }

    void TerrainManager::AddEmptyLayer() {
        _layers.emplace_back();
    }

    void TerrainManager::DeleteLayer(int layerIndex) {
        _layers.erase(_layers.begin() + layerIndex);
    }
}
