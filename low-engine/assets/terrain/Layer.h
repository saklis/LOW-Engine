#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "assets/animation/SpriteSheet.h"
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/Image.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/System/Vector2.hpp"

#include "LayerDefinition.h"

namespace LowEngine::Terrain {
    /**
     * @brief Represents the state of an animated tile.
     *
     * Used to manage and track the state of a tile that has animation properties.
     */
    class AnimatedTileState {
    public:
        /**
         * @brief Pointer to current Clip.
         */
        std::vector<Animation::AnimationClip*> Clips;

        /**
         * @brief Index of current clip.
         *
         * Used when particular cell has more than one Clip assigned to it.
         * This is assigned on load to esure that Clips don't switch randomly during gameplay.
         */
        size_t ClipIndex = 0;

        /**
         * @brief Number of current frame.
         */
        size_t CurrentFrame = 0;

        /**
         * @brief Timer for current frame.
         */
        float FrameTime = 0;
    };

    /**
     * @brief Terrain's layer. Tile Map can have multiple layers, each with designated purpose.
     */
    class Layer {
    public:
        /**
         * @brief Type of this Layer.
         */
        LayerType Type = LayerType::Terrain;

        /**
         * @brief Definition of this Layer.
         *
         * Contains information about texture, cell size, cell count and other properties.
		 */
		LayerDefinition Definition;

        /**
         * @brief ID of the texture that is used for this layer.
         *
         * This ID is used to retrieve the texture from the Assets manager.
		 */
        size_t TextureId = -1;

        /**
         * @brief Size of single cell, in pixels.
         */
        size_t CellSize = 0;

        /**
         * @brief Number of cells on this layer.
         */
        sf::Vector2<size_t> CellCount;

        /**
         * @brief Size of this layer, in pixels.
         */
        sf::Vector2<size_t> LayerSize;


        /**
         * @brief Index of the tile in the tileset.
         * index = y * CellCountX + x
         */
        std::vector<size_t> Cells;

        /**
         * @brief State of animated tiles.
         */
        std::unordered_map<size_t, AnimatedTileState> AnimatedTiles;

        /**
         * @brief Index of Animation Clip that was assigned to a cell.
         *
         * If particular cell has multiple Animation Clips assigned to it, on load one of the Clips will be selected at random.
         * Index of the selected Clip will be stored in this map.
         */
        std::unordered_map<size_t, size_t> CellClipIndex;

        /**
         * @brief Constructs a new Layer object with a default texture.
         *
         * This constructor initializes a Layer instance by associating it with a given texture.
         * The texture is used to define the appearance of this layer's graphical elements.
         *
         * @param defaultTexture A reference to the sf::Texture object used to initialize the layer's sprite.
         * It provides the graphical representation for this layer.
         */
        explicit Layer(const sf::Texture& defaultTexture) : _sprite(defaultTexture) {
        }

        ~Layer() = default;

        /**
         * @brief Apply a texture with provided ID as base image for this layer.
         *
         * Texture can be changed at any point.
         * @param textureId ID of the texture.
         */
        void LoadTexture(size_t textureId);

        /**
         * @brief Set size for the layer.
         * @param cellCount Number of cells.
         * @param cellSize Size of a cell, in pixels.
         */
        void SetSize(const sf::Vector2<size_t>& cellCount, const size_t& cellSize);

        /**
         * @brief Updates underlying Sprite object to reflect current state of animated tiles and return a pointer to the updated Sprite.
         * @return Pointer to updated Sprite. Nullpointer if generation failed.
         */
        sf::Sprite* GetDrawable();

    protected:
        sf::Image _sourceImage;
        sf::Image _image;
        sf::Texture _texture;
        sf::Sprite _sprite;
    };
}
