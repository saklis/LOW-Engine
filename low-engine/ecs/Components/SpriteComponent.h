#pragma once

#include "assets/Assets.h"

#include "SFML/Graphics/Texture.hpp"

#include "ecs/IComponent.h"
#include "ecs/Components/TransformComponent.h"
#include "graphics/Sprite.h"

namespace LowEngine::ECS {
    class SpriteComponent : public IComponent {
    public:
        unsigned int TextureId = 0;
        Sprite Sprite;
        int Layer = 0;

        explicit SpriteComponent(Memory::Memory* memory)
            : IComponent(memory), Sprite(Assets::GetDefaultTexture()) {
        }

        virtual ~SpriteComponent() = default;

        static const std::vector<std::type_index>& Dependencies() {
            static std::vector dependencies = {
                std::type_index(typeid(TransformComponent))
            };
            return dependencies;
        }

        void Initialize() override {
        }

        void Update(float deltaTime) override;

        LowEngine::Sprite* Draw() override {
            return &Sprite;
        }

        virtual void SetSprite(const std::string& textureAlias);

        virtual void SetSprite(int textureId);

    protected:
        virtual void SetSprite(const sf::Texture& texture);
    };
}
