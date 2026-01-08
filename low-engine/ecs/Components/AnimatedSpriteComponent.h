#pragma once

#include "SFML/Graphics/Texture.hpp"

#include "../../log/Log.h"
#include "ecs/IComponent.h"
#include "ecs/Components/TransformComponent.h"
#include "graphics/Sprite.h"
#include "assets/Assets.h"

namespace LowEngine::ECS {
    /**
     * @brief Represents a component that draws an animated sprite.
     *
     * Depends on Transform Component
     */
    class AnimatedSpriteComponent : public IComponent<AnimatedSpriteComponent, TransformComponent> {
    public:
        /**
         * @brief Id of the texture used by the Sprite.
         */
        size_t TextureId = 0;

        /**
         * @brief This component's Sprite.
         */
        LowEngine::Sprite Sprite;

        /**
         * @brief Layer number.
         *
         * Sprite of this component will be drawn on this layer.
         * This applies only if Scene's sorting mode is set to Layer.
         */
        int Layer = 0;

        /**
         * @brief Name of currently playing animation.
         */
        std::string CurrentClipName;

        /**
         * @brief Index of animation's current frame.
         */
        size_t CurrentFrame = 0;

        /**
         * @brief Timer for current frame.
         */
        float FrameTime = 0.0f;

        /**
         * @brief Should animation loop?
         *
         * If True, animation will restart from the begining after last frame.
         * If set to False, Animation will freeze on last frame.
         */
        bool Loop = true;

        explicit AnimatedSpriteComponent(Memory::Memory* memory)
            : IComponent(memory), Sprite(Assets::GetDefaultTexture()) {
        }

        AnimatedSpriteComponent(Memory::Memory* memory, AnimatedSpriteComponent const* other)
            : IComponent(memory, other),
              TextureId(other->TextureId), Sprite(other->Sprite), Layer(other->Layer),
              CurrentClipName(other->CurrentClipName),
              CurrentFrame(other->CurrentFrame), FrameTime(other->FrameTime), Loop(other->Loop) {
        }

        /**
         * @brief Set texture that will be used as source for animation frames.
         *
         * Setting Texture will automatically load its Sprite Sheet
         * @param textureAlias Texture's alias.
         */
        void SetTexture(const std::string& textureAlias);

        /**
         * @brief Set texture that will be used as source for animation frames.
         *
         * Setting Texture will automatically load its Sprite Sheet
         * @param textureId Texture's Id.
         */
        void SetTexture(size_t textureId);

        /**
         * @brief Play animation.
         *
         * Function will automatically find and load correct Animation Clip.
         * @param animationName Animation's name.
         * @param loop Should animation loop?
         */
        void Play(const std::string& animationName, bool loop = true);

        /**
         * @brief Stop currently playing animation.
         *
         * Function will clear Animation Clip.
         * When Animation is stopped, Sprite will freez on last displayed frame.
         */
        void Stop();

        void Initialize() override {
        }
        

        void Update(float deltaTime) override;

        LowEngine::Sprite* Draw() override {
            return &Sprite;
        }

        nlohmann::ordered_json SerializeToJSON() override;
        bool DeserializeFromJSON(const nlohmann::ordered_json& jsonData) override;

    protected:
        void SetTexture(const sf::Texture& texture);

        /**
         * @brief Updates Sprite properties to match selected Texture's properties.
         */
        void UpdateFrameSize();
    };
}
