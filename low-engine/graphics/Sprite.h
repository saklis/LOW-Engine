#pragma once
#include "assets/Assets.h"
#include "SFML/Graphics/Sprite.hpp"

namespace LowEngine {
    class Sprite : public sf::Sprite {
    public:
        /**
         * @brief Layer number.
         *
         * Sprite of this component will be drawn on this layer.
         * This applies only if Scene's sorting mode is set to Layer.
         */
        int Layer = 0;

        explicit Sprite(const sf::Texture& texture)
            : sf::Sprite(texture) {
        }

        explicit Sprite(const sf::Texture&& texture)
            : sf::Sprite(texture) {
        }

        Sprite(const sf::Texture& texture, const sf::IntRect& rectangle)
            : sf::Sprite(texture, rectangle) {
        }

        Sprite(const sf::Texture&& texture, const sf::IntRect& rectangle)
            : sf::Sprite(texture, rectangle) {
        }
    };
}
