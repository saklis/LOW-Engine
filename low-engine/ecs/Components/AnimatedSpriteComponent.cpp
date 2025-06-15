#include "AnimatedSpriteComponent.h"

namespace LowEngine::ECS {
    void AnimatedSpriteComponent::SetTexture(const std::string& textureAlias) {
        SpriteComponent::SetTexture(textureAlias);
        Sheet = Assets::GetSpriteSheet(textureAlias);

        UpdateFrameSize();
    }

    void AnimatedSpriteComponent::SetTexture(int32_t textureId) {
        SpriteComponent::SetTexture(textureId);
        Sheet = Assets::GetSpriteSheet(textureId);

        UpdateFrameSize();
    }

    void AnimatedSpriteComponent::Play(const std::string& animationName, bool loop) {
        if (Sheet == nullptr) {
            _log->error("Cannot play animation {}. No sprite sheet is not set.", animationName);
            return;
        }

        Clip = Sheet->GetAnimationClip(animationName);

        if (Clip == nullptr) {
            _log->error("Cannot play animation {}. Animation clip does not exist.", animationName);
            return;
        }

        CurrentFrame = 0;
        FrameTime = 0.0f;
        Loop = loop;

        Sprite.setTextureRect(Clip->Frames[CurrentFrame]);
    }

    void AnimatedSpriteComponent::Stop() {
        Clip = nullptr;
        CurrentFrame = 0;
        FrameTime = 0.0f;
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

        Sprite.setTextureRect(Clip->Frames[CurrentFrame]);
    }

    void AnimatedSpriteComponent::SetTexture(const sf::Texture& texture) {
        SpriteComponent::SetTexture(texture);
    }

    void AnimatedSpriteComponent::UpdateFrameSize() {
        auto size = sf::Vector2<int>(static_cast<int>(Sheet->FrameSize.x), static_cast<int>(Sheet->FrameSize.y));
        Sprite.setTextureRect(sf::IntRect({0, 0}, size));
        Sprite.setOrigin({static_cast<float>(size.x) / 2, static_cast<float>(size.y) / 2});
    }
}
