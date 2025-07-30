#pragma once
#include <nlohmann/json.hpp>

#include "Layer.h"
#include "navigation/NavigationGrid.h"

namespace LowEngine::Terrain {
    /**
     * @brief Describes a Tile Map with layers.
     */
    class TileMap {
    public:
        /**
         * @brief Name of this Tile Map
         */
        std::string Name;

        /**
         * @brief Path to the Tile Map file.
         *
         * This is the path from which the Tile Map was loaded, or where it should be saved.
		 */
        std::string Path;

        /**
         * @brief Size of the tile map, in cells.
         */
        sf::Vector2<size_t> Size;

        /**
         * @brief Navigation helper that holds navigation data generated from the map.
         *
         * It also provides functions to make use of the navigation data, like pathfinding.
         */
        Navigation::NavigationGrid NavGrid;

        /**
         * @brief Terrain layer of this Tile Map - ground, water,...
         */
        Layer TerrainLayer;

        /**
         * @brief Features layer of this Tile Map - mountains, forests, hills...
         */
        Layer FeaturesLayer;

        /**
         * @brief Constructs a TileMap with the specified default texture.
         *
         * Initializes the Terrain and Features layers of the TileMap by setting their default
         * texture and layer types.
         *
         * @param defaultTexture The default texture applied to the tile map layers.
         * @return An instance of the TileMap class with initialized layers.
         */
        explicit TileMap(const sf::Texture& defaultTexture) : TerrainLayer(defaultTexture), FeaturesLayer(defaultTexture) {
            TerrainLayer.Type = LayerType::Terrain;
            FeaturesLayer.Type = LayerType::Features;
        }

        void Update(float deltaTime);


        /**
         * @brief Load data from LDTk file (*.ldtkl) JSON data.
         *
         * Layer name in the JSON must be the same as LowEngine::Terrain::LayerType
         * @see LowEngine::Terrain::LayerType
         * @param path
         */
        void LoadFromLDTkJson(std::string path);
    };
}
