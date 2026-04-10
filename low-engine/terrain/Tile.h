#pragma once
#include <cstdint>
#include <string>

#include "SFML/Graphics/Rect.hpp"

namespace LowEngine::TileMap {
    enum class TileType : std::uint8_t {
        Static,
        Animated
    };

    class Tile {
    public:
        TileType Type = TileType::Static;
        sf::IntRect SpriteRect = sf::IntRect();

        std::string AnimationClipName = std::string();

        float AnimSpriteTimer = 0.0f;
        std::size_t AnimSpriteCurrentFrame = 0;

    protected:
    };
}
