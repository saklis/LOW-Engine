#pragma once

#include <cmath>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Rect.hpp"

namespace LowEngine::Animation {
    class SpriteSheet;
    /**
     * @brief Stores data about particular animation.
     */
    struct AnimationClip {
        /**
         * @brief Name of this animation.
         */
        std::string Name;

        /**
         * @brief Index of the starting frame for this animation.
         */
        size_t StartFrame = 0;

        /**
         * @brief Index of the last from for this animation.
         */
        size_t EndFrame = 0;

        /**
         * @brief Number of frames.
         */
        size_t FrameCount = 0;

        /**
         * @brief How long each frame should be displayed, in seconds.
         */
        float FrameDuration = 0.0f;

        /**
         * @brief Coordinates of orgin point (upper-left corner) for the first frame, in pixels.
         */
        sf::Vector2<size_t> FirstFrameOrigin;

        /**
         * @brief List of rectangles representing every frame of animation.
         */
        std::vector<sf::IntRect> Frames;


        /**
         * @brief Calculates and retrieves the frames per second (FPS) for the animation.
         * @return The frames per second (FPS) based on the frame duration of the animation.
         */
        [[nodiscard]] float GetFPS() const { return 1.0f / FrameDuration; }

        /**
         * @brief Calculates and retrieves the total duration of the animation in seconds.
         * @return The total duration of the animation derived from the frame duration and frame count.
         */
        [[nodiscard]] float GetAnimationDuration() const { return FrameDuration * static_cast<float>(FrameCount); }

        /**
         * @brief Recalculates the frames based on the new position of the first frame.
         * @param spriteSheet Reference to sprite sheet that will be used as base for recalculation.
         */
        void RecalculateFrames(const ::LowEngine::Animation::SpriteSheet& spriteSheet);
    };

    /**
     * @brief Holds information about frame sizes for a particular texture.
     */
    class SpriteSheet {
    public:
        /**
         * @brief ID of the texture this SpriteSheet applies to.
         */
        size_t TextureId = 0;

        /**
         * @brief Size of a single frame /cell in a sheet, in pixels.
         */
        sf::Vector2<size_t> FrameSize;

        /**
         * @brief Number of frames/cells on each axis.
         */
        sf::Vector2<size_t> FrameCount;

        /**
         * @brief Add a new animation definition to this Sheet.
         * @param name Name of the animation.
         * @param frameIndex Index of the first frame for the animation.
         * @param frameCount Number of frames in the animation.
         * @param frameDuration How long each frame should be displayed, in seconds.
         * @param firstFrameOrigin Coordinates for the origin point (upper-left corner) of the first animation frame, in pixels.
         */
        void AddAnimationClip(const std::string& name, size_t frameIndex, size_t frameCount, float frameDuration,
                              const sf::Vector2<size_t>& firstFrameOrigin);

        /**
         * @brief Check if animation with given name is defined for this Sheet.
         * @param name Name of the animation to check.
         * @return True if animation with given name is defined, false otherwise.
		 */
        bool HasAnimationClip(const std::string& name) const;

        /**
         * @brief Remove animation with given name from this Sheet.
         * @param name Name of the animation to remove.
         */
        void RemoveAnimationClip(const std::string& name);

        /**
         * @brief Retrieve the list of all animation names defined for this Sheet.
         * @return List of names of all defined animations.
         */
        std::vector<std::string> GetAnimationClipNames();

        /**
         * @brief Retrive Animation Clip by its name.
         * @param name Name of the Clip to retrieve.
         * @return Pointer to Clip. Nullptr if Clip was not found.
         */
        Animation::AnimationClip& GetAnimationClip(const std::string& name);

    protected:
        std::unordered_map<std::string, std::unique_ptr<AnimationClip>> _animations;
    };
}
