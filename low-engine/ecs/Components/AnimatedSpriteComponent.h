#pragma once

#include "Log.h"
#include "SpriteComponent.h"

namespace LowEngine::ECS {
    class AnimatedSpriteComponent : public SpriteComponent {
    public:
        Animation::AnimationSheet* Sheet = nullptr;
        Animation::AnimationClip* Clip = nullptr;

        unsigned int CurrentFrame = 0;
        float FrameTime = 0.0f;
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

        void SetSprite(const std::string& textureAlias) override;

        void SetSprite(int textureId) override;

        void Play(const std::string& animationName, bool loop = true);

        void Stop();

        void SetLooping(bool looping);

        void Initialize() override {
            SpriteComponent::Initialize();
        }

        void Update(float deltaTime) override;

        LowEngine::Sprite* Draw() override {
            return &Sprite;
        }

    protected
    :
        void SetSprite(const sf::Texture& texture) override;

        void UpdateFrameSize();
    };
}
