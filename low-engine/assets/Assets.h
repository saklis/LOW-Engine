#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <random>

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Image.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/System/Exception.hpp"
#include "nlohmann/json.hpp"

#include "Log.h"

#include "animation/AnimationSheet.h"
#include "terrain/Map.h"

#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Sprite.hpp"

#include "defaults/unitblock.hpp"
#include "terrain/LayerToTextureMapping.h"

namespace LowEngine {
    class Assets {
    public:
        static size_t LoadTexture(const std::string& path);

        static size_t LoadTexture(const std::string& path, const std::string& alias);

        static void LoadTextureWithAnimationSheet(const std::string& path, size_t frameWidth,
                                                  size_t frameHeight, size_t frameCountX,
                                                  size_t frameCountY);

        static void LoadTextureWithAnimationSheet(const std::string& path, const std::string& alias,
                                                  size_t frameWidth, size_t frameHeight,
                                                  size_t frameCountX, size_t frameCountY);

        static void AddAnimationSheet(size_t textureId, size_t frameWidth, size_t frameHeight,
                                      size_t frameCountX, size_t frameCountY);

        static void AddAnimationSheet(const std::string& textureAlias, size_t frameWidth,
                                      size_t frameHeight, size_t frameCountX, size_t frameCountY);

        static void AddAnimationClip(size_t textureId, const std::string& name, size_t firstFrameIndex,
                                     size_t frameCount, float frameDuration);

        static void AddAnimationClip(const std::string& textureAlias, const std::string& name,
                                     size_t firstFrameIndex, size_t frameCount, float frameDuration);

        static size_t LoadMap(const std::string& path, const std::vector<Terrain::LayerToTextureMapping>& mappings);
        static size_t LoadMap(const std::string& path, const std::string& alias, const std::vector<Terrain::LayerToTextureMapping>& mappings);

        static Terrain::Map& GetMap(size_t mapId);

        static Animation::AnimationSheet* GetAnimationSheet(size_t textureId);

        static Animation::AnimationSheet* GetAnimationSheet(const std::string& textureAlias);

        static sf::Texture& GetDefaultTexture();

        static sf::Texture& GetTexture(size_t textureId);

        static sf::Texture& GetTexture(const std::string& textureAlias);

        static size_t GetTextureId(const std::string& textureAlias);

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

        std::vector<Terrain::Map> _maps;
        std::unordered_map<std::string, size_t> _mapAliases;

        std::vector<sf::Texture> _textures;
        std::unordered_map<std::string, size_t> _textureAliases;
        std::unordered_map<size_t, Animation::AnimationSheet> _animationSheets;

        std::vector<sf::Font> _fonts;
        std::unordered_map<std::string, sf::Font> _fontAliases;
    };
}
