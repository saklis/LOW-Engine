#pragma once

#include <cmath>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "SFML/System/Vector2.hpp"

namespace LowEngine::Animation {
    struct AnimationClip {
        std::string Name;
        size_t StartFrame = 0;
        size_t EndFrame = 0;
        size_t FrameCount = 0;

        float FrameDuration = 0.0f;

        sf::Vector2<size_t> FirstFrameOrigin;

        [[nodiscard]] float GetFPS() const { return 1.0f / FrameDuration; }
    };

    class AnimationSheet {
    public:
        size_t TextureId = 0;

        size_t FrameWidth = 0;
        size_t FrameHeight = 0;

        size_t FrameCountX = 0;
        size_t FrameCountY = 0;

        void AddAnimationClip(const std::string& name, size_t frameIndex, size_t frameCount, float frameDuration, const sf::Vector2<size_t>& firstFrameOrigin);

        std::vector<std::string> GetAnimationClipNames();
        AnimationClip* GetAnimationClip(const std::string& name);

    protected:
        std::unordered_map<std::string, AnimationClip> _animations;
    };
}
