#pragma once

#include "Log.h"
#include "SpriteComponent.h"

namespace LowEngine::ECS {
    /**
     * @brief Represents a component that draws an animated sprite.
     *
     * Depends on Transform Component
     */
    class AnimatedSpriteComponent : public SpriteComponent {
    public:
        /**
         * @brief Pointer to Sprite Sheet that should be source of the animation frames.
         */
        Animation::SpriteSheet* Sheet = nullptr;

        /**
         * @brief Pointer to Animation Clip that describes currently selected animation.
         */
        Animation::AnimationClip* Clip = nullptr;

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

        static const std::vector<std::type_index>& Dependencies() {
            static std::vector dependencies = {
                std::type_index(typeid(TransformComponent))
            };
            return dependencies;
        }

        explicit AnimatedSpriteComponent(Memory::Memory* memory)
            : SpriteComponent(memory) {
        }

        AnimatedSpriteComponent(Memory::Memory* memory, AnimatedSpriteComponent const* other)
            : SpriteComponent(memory, other),
              Sheet(other->Sheet), Clip(other->Clip),
              CurrentFrame(other->CurrentFrame), FrameTime(other->FrameTime), Loop(other->Loop) {
        }

        void CloneInto(Memory::Memory* newMemory, void* rawStorage) const override {
            new(rawStorage) AnimatedSpriteComponent(newMemory, this);
        }

        /**
         * @brief Set texture that will be used as source for animation frames.
         *
         * Setting Texture will automatically load its Sprite Sheet
         * @param textureAlias Texture's alias.
         */
        void SetTexture(const std::string& textureAlias) override;

        /**
         * @brief Set texture that will be used as source for animation frames.
         *
         * Setting Texture will automatically load its Sprite Sheet
         * @param textureId Texture's Id.
         */
        void SetTexture(size_t textureId) override;

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
            SpriteComponent::Initialize();
        }

        void Update(float deltaTime) override;

        LowEngine::Sprite* Draw() override {
            return &Sprite;
        }

    protected:
        void SetTexture(const sf::Texture& texture) override;

        /**
         * @brief Updates Sprite properties to match selected Texture's properties.
         */
        void UpdateFrameSize();
    };
}
