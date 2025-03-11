#include "AnimatedSpriteComponent.h"

namespace LowEngine::ECS {
    void AnimatedSpriteComponent::SetSprite(const std::string& textureAlias) {
        SpriteComponent::SetSprite(textureAlias);
        _animationSheet = Assets::GetAnimationSheet(textureAlias);
    }

    void AnimatedSpriteComponent::SetSprite(int32_t textureId) {
        SpriteComponent::SetSprite(textureId);
        _animationSheet = Assets::GetAnimationSheet(textureId);
    }

    void AnimatedSpriteComponent::Play(const std::string& animationName, bool loop) {
        if (_animationSheet == nullptr) {
            _log->error("Cannot play animation {}. No animation sheet or clip set.", animationName);
            return;
        }

        _currentClip = _animationSheet->GetAnimationClip(animationName);

        _currentFrame = 0;
        _frameTime = 0.0f;
        _loop = loop;

        sf::IntRect frame;
        frame.position.x = _currentClip->StartFrame % _animationSheet->FrameCountX * _animationSheet->FrameWidth;
        frame.position.y = _currentClip->StartFrame / _animationSheet->FrameCountX * _animationSheet->FrameHeight;
        frame.size.x = _animationSheet->FrameWidth;
        frame.size.y = _animationSheet->FrameHeight;

        Sprite.setTextureRect(frame);
    }

    void AnimatedSpriteComponent::SetLooping(bool looping) {
        _loop = looping;
    }

    void AnimatedSpriteComponent::Update(float deltaTime) {
        SpriteComponent::Update(deltaTime);

        if (_animationSheet == nullptr || _currentClip == nullptr) return;

        _frameTime += deltaTime;
        if (_frameTime >= _currentClip->FrameDuration) {
            _frameTime = 0.0f;
            _currentFrame++;
            if (_currentFrame >= _currentClip->FrameCount) {
                if (_loop) {
                    _currentFrame = 0;
                } else {
                    _currentClip = nullptr;
                    return;
                }
            }
        }

        sf::IntRect frame;
        frame.position.x = (_currentClip->StartFrame + _currentFrame) % _animationSheet->FrameCountX * _animationSheet->
                           FrameWidth;
        frame.position.y = _currentClip->StartFrame / _animationSheet->FrameCountX * _animationSheet->FrameHeight;
        frame.size.x = _animationSheet->FrameWidth;
        frame.size.y = _animationSheet->FrameHeight;

        Sprite.setTextureRect(frame);
    }

    void AnimatedSpriteComponent::SetSprite(const sf::Texture& texture) {
        SpriteComponent::SetSprite(texture);
    }
}
