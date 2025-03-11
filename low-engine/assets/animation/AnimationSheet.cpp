#include "AnimationSheet.h"

namespace LowEngine::Animation {
    void AnimationSheet::AddAnimationClip(const std::string& name, uint32_t frameIndex, uint32_t frameCount, float_t frameDuration) {
        AnimationClip& anim = _animations[name];
        anim.StartFrame = frameIndex;
        anim.FrameCount = frameCount;
        anim.EndFrame = frameIndex + frameCount - 1;
        anim.FrameDuration = frameDuration;
    }

    Animation::AnimationClip* AnimationSheet::GetAnimationClip(const std::string& name) {
        if (_animations.find(name) == _animations.end()) return nullptr;
        return &_animations[name];
    }
}
