#pragma once

#include "assets/Assets.h"

#include "SFML/Graphics/Texture.hpp"

#include "ecs/IComponent.h"
#include "ecs/Components/TransformComponent.h"
#include "graphics/Sprite.h"

namespace LowEngine::ECS {
    /**
     * Represents a component that displays a Sprite.
     *
     * Depends on Transform Component
     */
    class SpriteComponent : public IComponent {
    public:
        /**
         * @brief Id of the texture used by the Sprite.
         */
        size_t TextureId = 0;

        /**
         * @brief This component's Sprite.
         */
        Sprite Sprite;

        /**
         * @brief Layer number.
         *
         * Sprite of this component will be drawn on this layer.
         * This applies only if Scene's sorting mode is set to Layer.
         */
        int Layer = 0;

        explicit SpriteComponent(Memory::Memory* memory)
            : IComponent(memory), Sprite(Assets::GetDefaultTexture()) {
        }

        SpriteComponent(Memory::Memory* memory, SpriteComponent const* other)
            : IComponent(memory, other), TextureId(other->TextureId), Sprite(other->Sprite), Layer(other->Layer) {
        }

        virtual ~SpriteComponent() = default;

        void CloneInto(Memory::Memory* newMemory, void* rawStorage) const override {
            new(rawStorage) SpriteComponent(newMemory, this);
        }

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

        /**
         * @brief Changes the texture the Sprite is using.
         * @param textureAlias Texture's alias.
         */
        virtual void SetTexture(const std::string& textureAlias);

        /**
         * @brief Changes the texture the Sprite is using.
         * @param textureId Texture's Id.
         */
        virtual void SetTexture(size_t textureId);

    protected:
        /**
         * @brief Changes the texture the Sprite is using.
         * @param texture Reference to texture.
         */
        virtual void SetTexture(const sf::Texture& texture);
    };
}
