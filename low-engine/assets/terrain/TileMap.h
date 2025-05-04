#pragma once
#include <nlohmann/json.hpp>

#include "Layer.h"

namespace LowEngine::Terrain {
    class TileMap {
    public:
        std::string Name;
        size_t Width = 0;
        size_t Height = 0;

        Layer TerrainLayer;
        Layer FeaturesLayer;

        explicit TileMap(const sf::Texture& defaultTexture) : TerrainLayer(defaultTexture), FeaturesLayer(defaultTexture) {
            TerrainLayer.Type = Terrain;
            FeaturesLayer.Type = Features;
        }

        void Update(float deltaTime);

        void LoadFromLDTkJson(nlohmann::json::const_reference jsonData);
    };
}
