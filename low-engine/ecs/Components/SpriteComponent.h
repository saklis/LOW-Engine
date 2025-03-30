#pragma once

#include "assets/Assets.h"

#include "ecs/IComponent.h"
#include "ecs/Components/TransformComponent.h"

#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Sprite.hpp"

namespace LowEngine::ECS {
    class SpriteComponent : public IComponent {
    public:
        unsigned int TextureId = 0;
        sf::Sprite Sprite;

        SpriteComponent();

        virtual ~SpriteComponent() = default;

        static const std::vector<std::type_index>& Dependencies() {
            static std::vector<std::type_index> dependencies = {
                std::type_index(typeid(TransformComponent))
            };
            return dependencies;
        }

        void Initialize() override {
        }

        void Update(float deltaTime) override;

        virtual void SetSprite(const std::string& textureAlias);

        virtual void SetSprite(int textureId);

    protected:
        virtual void SetSprite(const sf::Texture& texture);
    };
}
