#include "AnimatedSpriteComponent.h"

namespace LowEngine::ECS {
    void AnimatedSpriteComponent::SetTexture(const std::string& textureAlias) {
        if (!Assets::HasSpriteSheet(textureAlias)) {
            _log->error("Cannot set texture alias {}. No sprite sheet with this alias exists.", textureAlias);
            throw std::runtime_error("No sprite sheet with this alias exists");
        }

        SpriteComponent::SetTexture(textureAlias);
        UpdateFrameSize();
    }

    void AnimatedSpriteComponent::SetTexture(size_t textureId) {
        if (!Assets::HasSpriteSheet(textureId)) {
            _log->error("Cannot set texture alias {}. No sprite sheet with this alias exists.", textureId);
            throw std::runtime_error("No sprite sheet with this alias exists");
        }

        SpriteComponent::SetTexture(textureId);
        UpdateFrameSize();
    }

    void AnimatedSpriteComponent::Play(const std::string& animationName, bool loop) {
        if (Assets::HasSpriteSheet(TextureId) == false) {
            _log->error("Cannot play animation {}. No sprite sheet is not set.", animationName);
            return;
        }
        auto& Sheet = Assets::GetSpriteSheet(TextureId);

        if (Sheet.HasAnimationClip(animationName) == false) {
            _log->error("Cannot play animation {}. Animation clip does not exist.", animationName);
            return;
        }
        auto& Clip = Sheet.GetAnimationClip(animationName);

        CurrentClipName = animationName;
        CurrentFrame = 0;
        FrameTime = 0.0f;
        Loop = loop;

        Sprite.setTextureRect(Clip.Frames[CurrentFrame]);
    }

    void AnimatedSpriteComponent::Stop() {
        CurrentClipName.clear();
        CurrentFrame = 0;
        FrameTime = 0.0f;
    }

    void AnimatedSpriteComponent::Update(float deltaTime) {
        SpriteComponent::Update(deltaTime);

        if (CurrentClipName.empty()) return;

        auto& Sheet = Assets::GetSpriteSheet(TextureId);
        auto& Clip = Sheet.GetAnimationClip(CurrentClipName);

        FrameTime += deltaTime;
        if (FrameTime >= Clip.FrameDuration) {
            FrameTime = 0.0f;
            CurrentFrame++;
            if (CurrentFrame >= Clip.FrameCount) {
                if (Loop) {
                    CurrentFrame = 0;
                } else {
                    CurrentClipName.clear();
                    return;
                }
            }
        }

        Sprite.setTextureRect(Clip.Frames[CurrentFrame]);
    }

    void AnimatedSpriteComponent::SetTexture(const sf::Texture& texture) {
        SpriteComponent::SetTexture(texture);
    }

    void AnimatedSpriteComponent::UpdateFrameSize() {
        auto& sheet = Assets::GetSpriteSheet(TextureId);
        auto size = sf::Vector2<int>(static_cast<int>(sheet.FrameSize.x), static_cast<int>(sheet.FrameSize.y));
        Sprite.setTextureRect(sf::IntRect({0, 0}, size));
        Sprite.setOrigin({static_cast<float>(size.x) / 2.0f, static_cast<float>(size.y) / 2.0f});
    }
}
