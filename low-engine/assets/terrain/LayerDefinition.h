#pragma once
#include <string>
#include <vector>
#include <utility>

#include "Layer.h"

namespace LowEngine::Terrain {
    /**
     * @brief Structure used to describe a particular Cell (by its index).
     *
     * Those properties include navigation properties and possible animation clips
     */
    class CellDefinition {
    public:
        bool IsWalkable = false;
        bool IsSwimmable = false;
        bool IsFlyable = false;
        float MoveCost = 1.0f;

        /**
         * @brief Names of the Animation Clips defined for the texture.
         *
         * This is the way to make animated tiles.
         * First vector is the index of the cell. Multiple animations can be assigned to each index - in that case animation will be selected at random.
         * If no animation is provided for a cell, then it's treated as static tile.
         */
        std::vector<std::string> AnimationClipNames;

        CellDefinition(bool isWalkable, bool isSwimmable, bool isFlyable, float moveCost, const std::vector<std::string>& animationClipNames)
            : IsWalkable(isWalkable),
              IsSwimmable(isSwimmable),
              IsFlyable(isFlyable),
              MoveCost(moveCost),
              AnimationClipNames(animationClipNames) {
        }
    };

    /**
     * @brief A structure used while loading a Tile Map to provide texture and animation information.
     */
    class LayerDefinition {
    public:
        /**
         * @brief Type of this layer.
         */
        LayerType Type;

        /**
         * @brief ID of the texture that should be used.
         */
        size_t TextureId = 0;

        /**
         * @brief Structure holding properties for particular cell types, recognised by index.
         */
        std::unordered_map<unsigned, CellDefinition> CellDefinitions;

        /**
         * @brief Constructs a LayerToTextureMapping object with layer type, texture ID, and optional animation clip names.
         *
         * @param type The type of layer.
         * @param textureId The ID of the texture associated with this layer.
         * @param cellDefinitions Structure containing properties for particular cell, organized by cell's index.
         */
        LayerDefinition(LayerType type, size_t textureId, std::unordered_map<unsigned, CellDefinition> cellDefinitions = {})
            : Type(type),
              TextureId(textureId),
              CellDefinitions(std::move(cellDefinitions)) {
        }
    };
}
