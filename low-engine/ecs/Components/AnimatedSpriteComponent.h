#pragma once

#include "Log.h"
#include "SpriteComponent.h"

namespace LowEngine::ECS {
    class AnimatedSpriteComponent : public SpriteComponent {
    public:
        void SetSprite(const std::string& textureAlias) override;

        void SetSprite(int32_t textureId) override;

        void Play(const std::string& animationName, bool loop = true);
        void SetLooping(bool looping);

        void Update(float deltaTime) override;

    protected:
        Animation::AnimationSheet* _animationSheet = nullptr;
        Animation::AnimationClip* _currentClip = nullptr;

        uint32_t _currentFrame = 0;
        float _frameTime = 0.0f;
        bool _loop = true;

        void SetSprite(const sf::Texture& texture) override;
    };
}
