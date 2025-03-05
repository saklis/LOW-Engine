#pragma once

#include "LowHeaders.h"

#include "ecs/IComponent.h"
#include "ecs/Components/TransformComponent.h"

#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Sprite.hpp"

namespace LowEngine::ECS {
    class SpriteComponent : public IComponent {
    public:
        sf::Sprite Sprite;

        SpriteComponent();

        virtual ~SpriteComponent() = default;

        void Initialize() override {
        }

        void Update(float deltaTime) override {
        }

        void SetSprite(const sf::Texture& texture);
        void SetSprite(const std::string& textureAlias);
        void SetSprite(int32_t textureId);

        void ApplyTransforms(const LowEngine::ECS::TransformComponent& transforms);
    };
}
