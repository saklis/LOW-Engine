#pragma once

#include "assets/Assets.h"

#include "ecs/IComponent.h"
#include "ecs/Components/TransformComponent.h"

#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Sprite.hpp"

namespace LowEngine::ECS {
    class SpriteComponent : public IComponent {
    public:
        uint32_t TextureId = 0;
        sf::Sprite Sprite;

        SpriteComponent();

        virtual ~SpriteComponent() = default;

        void Initialize() override {
        }

        void Update(float deltaTime) override {
        }

        virtual void SetSprite(const std::string& textureAlias);
        virtual void SetSprite(int32_t textureId);

        void ApplyTransforms(const LowEngine::ECS::TransformComponent* transforms);

    protected:
        virtual void SetSprite(const sf::Texture& texture);
    };
}
