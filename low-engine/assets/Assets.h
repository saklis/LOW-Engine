#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Image.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/System/Exception.hpp"

#include "Log.h"

#include "animation/AnimationSheet.h"

namespace LowEngine {

    class Assets {
    public:
        static int32_t LoadTexture(const std::string& path);
        static int32_t LoadTexture(const std::string& path, const std::string& alias);

        static void LoadTextureWithAnimationSheet(const std::string& path, uint32_t frameWidth, uint32_t frameHeight, uint32_t frameCountX, uint32_t frameCountY);
        static void LoadTextureWithAnimationSheet(const std::string& path, const std::string& alias, uint32_t frameWidth, uint32_t frameHeight, uint32_t frameCountX, uint32_t frameCountY);

        static void AddAnimationSheet(int32_t textureId, uint32_t frameWidth, uint32_t frameHeight, uint32_t frameCountX, uint32_t frameCountY);
        static void AddAnimationSheet(const std::string& textureAlias, uint32_t frameWidth, uint32_t frameHeight, uint32_t frameCountX, uint32_t frameCountY);
        static void AddAnimationClip(int32_t textureId, const std::string& name, uint32_t firstFrameIndex, uint32_t frameCount, float_t frameDuration);
        static void AddAnimationClip(const std::string& textureAlias, const std::string& name, uint32_t firstFrameIndex, uint32_t frameCount, float_t frameDuration);

        static Animation::AnimationSheet* GetAnimationSheet(int32_t textureId);
        static Animation::AnimationSheet* GetAnimationSheet(const std::string& textureAlias);

        static sf::Texture& GetDefaultTexture();
        static sf::Texture& GetTexture(int32_t textureId);
        static sf::Texture& GetTexture(const std::string& textureAlias);

        static void UnloadAll();

    protected:
        Assets();
        Assets(const Assets&) = delete;
        Assets& operator=(const Assets&) = delete;

        static Assets* GetInstance() {
            static Assets instance;
            return &instance;
        }

        std::vector<sf::Texture> _textures;
        std::unordered_map<std::string, int32_t> _textureAliases;
        std::unordered_map<uint32_t, Animation::AnimationSheet> _animationSheets;
    };

}
