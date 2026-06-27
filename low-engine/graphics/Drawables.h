#pragma once

#include <variant>

#include "SFML/Graphics/VertexArray.hpp"
#include "SFML/Graphics/Texture.hpp"

#include "graphics/Sprite.h"

namespace LowEngine {
    /**
     * @brief Drawable that wraps an sf::VertexArray and its associated texture.
     *
     * Emitted by TileMapLayer for animated tiles. Rendered in Scene::Draw
     * as part of the unified sorted drawable pass.
     */
    struct VertexArrayDrawable {
        const sf::VertexArray* vertices = nullptr;
        const sf::Texture*     texture  = nullptr;
        int                    DrawOrder = 0;
    };

    /**
     * @brief Unified drawable type for the scene render pass.
     *
     * Holds either a Sprite or a VertexArrayDrawable. All drawables are
     * collected into vector<SceneDrawable>, sorted by DrawOrder, then drawn.
     */
    using SceneDrawable = std::variant<Sprite, VertexArrayDrawable>;
}