#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "assets/animation/AnimationSheet.h"
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/Image.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/System/Vector2.hpp"

namespace LowEngine::Terrain {
    enum LayerType {
        Terrain,
        Features,
    };

    class AnimatedTileState {
    public:
        std::vector<Animation::AnimationClip*> Clips;
        size_t ClipIndex = 0;
        size_t CurrentFrame = 0;
        float FrameTime = 0;
    };

    class Layer {
    public:
        LayerType Type;
        sf::Vector2<size_t> CellCount;
        sf::Vector2<size_t> LayerSize;

        size_t CellSize = 0;

        /**
         * The cells contain index of the tile in the tileset.
         * index = y * CellCountX + x
         */
        std::vector<size_t> Cells;
        std::unordered_map<size_t, AnimatedTileState> AnimatedTiles;
        std::unordered_map<size_t, size_t> CellClipIndex;

        explicit Layer(const sf::Texture& defaultTexture) : m_Sprite(defaultTexture) {
        }

        ~Layer() = default;

        void LoadTexture(size_t textureId);

        void SetSize(const sf::Vector2<size_t>& cellCount, const size_t& cellSize);

        sf::Drawable* GetDrawable();

    protected:
        size_t m_TextureId = -1;
        sf::Image m_SourceImage;
        sf::Image m_Image;
        sf::Texture m_Texture;
        sf::Sprite m_Sprite;
    };
}
