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

        void SetSprite(const std::string& textureAlias) override;
        void SetSprite(int textureId) override;

        void Play(const std::string& animationName, bool loop = true);
        void Stop();
        void SetLooping(bool looping);

        void Update(float deltaTime) override;

    protected:
        void SetSprite(const sf::Texture& texture) override;
        void UpdateFrameSize();
    };
}
