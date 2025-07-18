#include "SpriteSheet.h"

namespace LowEngine::Animation {
    void SpriteSheet::AddAnimationClip(const std::string& name, size_t frameIndex, size_t frameCount, float frameDuration,
                                       const sf::Vector2<size_t>& firstFrameOrigin) {
        AnimationClip& anim = _animations[name];
        anim.Name = name;
        anim.StartFrame = frameIndex;
        anim.FrameCount = frameCount;
        anim.EndFrame = frameIndex + frameCount - 1;
        anim.FrameDuration = frameDuration;
        anim.FirstFrameOrigin = firstFrameOrigin;

        anim.Frames.clear();
        anim.Frames.resize(anim.FrameCount);
        for (size_t i = 0; i < anim.FrameCount; i++) {
            sf::IntRect frame;
            frame.position.x = static_cast<int>((anim.StartFrame + i) % this->FrameCount.x * this->FrameSize.x);
            frame.position.y = static_cast<int>(anim.StartFrame / this->FrameCount.x * this->FrameSize.y);
            frame.size.x = static_cast<int>(this->FrameSize.x);
            frame.size.y = static_cast<int>(this->FrameSize.y);

            anim.Frames[i] = frame;
        }
    }

    std::vector<std::string> SpriteSheet::GetAnimationClipNames() {
        std::vector<std::string> names;
        for (auto& animation: _animations) {
            names.emplace_back(animation.first);
        }
        return names;
    }

    Animation::AnimationClip* SpriteSheet::GetAnimationClip(const std::string& name) {
        if (_animations.find(name) == _animations.end()) return nullptr;
        return &_animations[name];
    }
}
