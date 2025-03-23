#include "AnimatedSpriteComponent.h"

namespace LowEngine::ECS {
    void AnimatedSpriteComponent::SetSprite(const std::string& textureAlias) {
        SpriteComponent::SetSprite(textureAlias);
        Sheet = Assets::GetAnimationSheet(textureAlias);

        UpdateFrameSize();
    }

    void AnimatedSpriteComponent::SetSprite(int32_t textureId) {
        SpriteComponent::SetSprite(textureId);
        Sheet = Assets::GetAnimationSheet(textureId);

        UpdateFrameSize();
    }

    void AnimatedSpriteComponent::Play(const std::string& animationName, bool loop) {
        if (Sheet == nullptr) {
            _log->error("Cannot play animation {}. No animation sheet or clip set.", animationName);
            return;
        }

        Clip = Sheet->GetAnimationClip(animationName);

        CurrentFrame = 0;
        FrameTime = 0.0f;
        Loop = loop;

        sf::IntRect frame;
        frame.position.x = static_cast<int>(Clip->StartFrame % Sheet->FrameCountX * Sheet->FrameWidth);
        frame.position.y = static_cast<int>(Clip->StartFrame / Sheet->FrameCountX * Sheet->FrameHeight);
        frame.size.x = static_cast<int>(Sheet->FrameWidth);
        frame.size.y = static_cast<int>(Sheet->FrameHeight);

        Sprite.setTextureRect(frame);
    }

    void AnimatedSpriteComponent::Stop() {
        Clip = nullptr;
        CurrentFrame = 0;
        FrameTime = 0.0f;
    }

    void AnimatedSpriteComponent::SetLooping(bool looping) {
        Loop = looping;
    }

    void AnimatedSpriteComponent::Update(float deltaTime) {
        SpriteComponent::Update(deltaTime);

        if (Sheet == nullptr || Clip == nullptr) return;

        FrameTime += deltaTime;
        if (FrameTime >= Clip->FrameDuration) {
            FrameTime = 0.0f;
            CurrentFrame++;
            if (CurrentFrame >= Clip->FrameCount) {
                if (Loop) {
                    CurrentFrame = 0;
                } else {
                    Clip = nullptr;
                    return;
                }
            }
        }

        sf::IntRect frame;
        frame.position.x = static_cast<int>((Clip->StartFrame + CurrentFrame) % Sheet->FrameCountX * Sheet->FrameWidth);
        frame.position.y = static_cast<int>(Clip->StartFrame / Sheet->FrameCountX * Sheet->FrameHeight);
        frame.size.x = static_cast<int>(Sheet->FrameWidth);
        frame.size.y = static_cast<int>(Sheet->FrameHeight);

        Sprite.setTextureRect(frame);
    }

    void AnimatedSpriteComponent::SetSprite(const sf::Texture& texture) {
        SpriteComponent::SetSprite(texture);
    }

    void AnimatedSpriteComponent::UpdateFrameSize() {
        auto size = sf::Vector2<int>(Sheet->FrameWidth, Sheet->FrameHeight);
        Sprite.setTextureRect(sf::IntRect({0, 0}, size));
        Sprite.setOrigin({static_cast<float>(size.x) / 2, static_cast<float>(size.y) / 2});
    }
}
