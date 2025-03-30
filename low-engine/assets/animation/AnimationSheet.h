#pragma once

#include <cmath>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace LowEngine::Animation {
    struct AnimationClip {
        std::string Name;
        unsigned int StartFrame = 0;
        unsigned int EndFrame = 0;
        unsigned int FrameCount = 0;

        float FrameDuration = 0.0f;

        [[nodiscard]] float GetFPS() const { return 1.0f / FrameDuration; }
    };

    class AnimationSheet {
    public:
        unsigned int TextureId = 0;

        unsigned int FrameWidth = 0;
        unsigned int FrameHeight = 0;

        unsigned int FrameCountX = 0;
        unsigned int FrameCountY = 0;

        void AddAnimationClip(const std::string& name, unsigned int frameIndex, unsigned int frameCount, float frameDuration);

        std::vector<std::string> GetAnimationClipNames();
        AnimationClip* GetAnimationClip(const std::string& name);

    protected:
        std::unordered_map<std::string, AnimationClip> _animations;
    };
}
