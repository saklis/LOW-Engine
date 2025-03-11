#pragma once

#include <cmath>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace LowEngine::Animation {
    struct AnimationClip {
        uint32_t StartFrame = 0;
        uint32_t EndFrame = 0;
        uint32_t FrameCount = 0;

        float_t FrameDuration = 0.0f;

        [[nodiscard]] float GetFPS() const { return 1.0f / FrameDuration; }
    };

    class AnimationSheet {
    public:
        uint32_t TextureId = 0;

        uint32_t FrameWidth = 0;
        uint32_t FrameHeight = 0;

        uint32_t FrameCountX = 0;
        uint32_t FrameCountY = 0;

        void AddAnimationClip(const std::string& name, uint32_t frameIndex, uint32_t frameCount, float_t frameDuration);

        Animation::AnimationClip* GetAnimationClip(const std::string& name);

    protected:
        std::unordered_map<std::string, AnimationClip> _animations;
    };
}
