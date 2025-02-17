#pragma once

#include "Config.h"
#include "ecs/IComponent.h"
#include "SFML/Graphics/Texture.hpp"

namespace LowEngine::ECS {
    class SpriteComponent : public IComponent {
    public:
        SpriteComponent() : _sprite() {
        }

        virtual ~SpriteComponent() = default;

        void Initialize() override {
        }

        void Update(float deltaTime) override {
        }

        void SetSprite(const sf::Texture& texture);

    protected:
        sf::Sprite _sprite;
    };
}
