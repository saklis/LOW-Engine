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
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Sprite.hpp"

#include "defaults/unitblock.hpp"

namespace LowEngine {
    class Assets {
    public:
        static int LoadTexture(const std::string& path);

        static int LoadTexture(const std::string& path, const std::string& alias);

        static void LoadTextureWithAnimationSheet(const std::string& path, unsigned int frameWidth,
                                                  unsigned int frameHeight, unsigned int frameCountX,
                                                  unsigned int frameCountY);

        static void LoadTextureWithAnimationSheet(const std::string& path, const std::string& alias,
                                                  unsigned int frameWidth, unsigned int frameHeight,
                                                  unsigned int frameCountX, unsigned int frameCountY);

        static void AddAnimationSheet(int32_t textureId, unsigned int frameWidth, unsigned int frameHeight,
                                      unsigned int frameCountX, unsigned int frameCountY);

        static void AddAnimationSheet(const std::string& textureAlias, unsigned int frameWidth,
                                      unsigned int frameHeight, unsigned int frameCountX, unsigned int frameCountY);

        static void AddAnimationClip(int32_t textureId, const std::string& name, unsigned int firstFrameIndex,
                                     unsigned int frameCount, float frameDuration);

        static void AddAnimationClip(const std::string& textureAlias, const std::string& name,
                                     unsigned int firstFrameIndex, unsigned int frameCount, float frameDuration);

        static Animation::AnimationSheet* GetAnimationSheet(int textureId);

        static Animation::AnimationSheet* GetAnimationSheet(const std::string& textureAlias);

        static sf::Texture& GetDefaultTexture();

        static sf::Texture& GetTexture(int textureId);

        static sf::Texture& GetTexture(const std::string& textureAlias);

        static sf::Font& GetDefaultFont();

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
        std::unordered_map<std::string, int> _textureAliases;
        std::unordered_map<unsigned int, Animation::AnimationSheet> _animationSheets;

        std::vector<sf::Font> _fonts;
        std::unordered_map<std::string, sf::Font> _fontAliases;
    };
}
