#include "AnimationSheet.h"

namespace LowEngine::Animation {
    void AnimationSheet::AddAnimationClip(const std::string& name, unsigned int frameIndex, unsigned int frameCount, float frameDuration) {
        AnimationClip& anim = _animations[name];
        anim.Name = name;
        anim.StartFrame = frameIndex;
        anim.FrameCount = frameCount;
        anim.EndFrame = frameIndex + frameCount - 1;
        anim.FrameDuration = frameDuration;
    }

    std::vector<std::string> AnimationSheet::GetAnimationClipNames() {
        std::vector<std::string> names;
        for (auto& animation : _animations) {
            names.emplace_back(animation.first);
        }
        return names;
    }

    Animation::AnimationClip* AnimationSheet::GetAnimationClip(const std::string& name) {
        if (_animations.find(name) == _animations.end()) return nullptr;
        return &_animations[name];
    }
}
