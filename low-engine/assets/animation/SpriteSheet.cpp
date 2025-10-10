#include "SpriteSheet.h"

#include "../../log/Log.h"

namespace LowEngine::Animation {
    void AnimationClip::RecalculateFrames(const Animation::SpriteSheet& spriteSheet) {
        if (Frames.size() != FrameCount) {
            Frames.resize(FrameCount);
        }

        auto firstFrame = Frames[0];
        for (size_t i = 1; i < FrameCount; i++) {
            Frames[i].position.x = firstFrame.position.x + (i * spriteSheet.FrameSize.x);
            Frames[i].position.y = firstFrame.position.y;
            Frames[i].size.x = spriteSheet.FrameSize.x;
            Frames[i].size.y = spriteSheet.FrameSize.y;
        }

        StartFrame = Frames[0].position.x / spriteSheet.FrameSize.x + (Frames[0].position.y / spriteSheet.FrameSize.y) * spriteSheet.FrameCount.x;
        EndFrame = StartFrame + FrameCount - 1;
    }

    void SpriteSheet::AddAnimationClip(const std::string& name, size_t frameIndex, size_t frameCount, float frameDuration,
                                       const sf::Vector2<size_t>& firstFrameOrigin) {
        if (!HasAnimationClip(name)) {
            auto anim = std::make_unique<AnimationClip>();
            anim->Name = name;
            anim->StartFrame = frameIndex;
            anim->FrameCount = frameCount;
            anim->EndFrame = frameIndex + frameCount - 1;
            anim->FrameDuration = frameDuration;
            anim->FirstFrameOrigin = firstFrameOrigin;

            anim->Frames.clear();
            anim->Frames.resize(anim->FrameCount);
            for (size_t i = 0; i < anim->FrameCount; i++) {
                sf::IntRect frame;
                frame.position.x = static_cast<int>((anim->StartFrame + i) % this->FrameCount.x * this->FrameSize.x);
                frame.position.y = static_cast<int>(anim->StartFrame / this->FrameCount.x * this->FrameSize.y);
                frame.size.x = static_cast<int>(this->FrameSize.x);
                frame.size.y = static_cast<int>(this->FrameSize.y);

                anim->Frames[i] = frame;
            }
            _animations[name] = std::move(anim);
            _log->debug("Animation clip added with name: '{}'", name);
        } else {
            _log->error("Animation clip with name '{}' already exists!", name);
        }
    }

    bool SpriteSheet::HasAnimationClip(const std::string& name) const {
        return _animations.contains(name);
    }

    void SpriteSheet::RemoveAnimationClip(const std::string& name) {
        _animations.erase(name);
    }

    std::vector<std::string> SpriteSheet::GetAnimationClipNames() {
        std::vector<std::string> names;
        for (auto& animation: _animations) {
            names.emplace_back(animation.first);
        }
        return names;
    }

    Animation::AnimationClip& SpriteSheet::GetAnimationClip(const std::string& name) {
        return *_animations[name];
    }
}
