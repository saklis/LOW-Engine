#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "SFML/Graphics/Texture.hpp"
#include "SFML/System/Exception.hpp"

#include "LowHeaders.h"

namespace LowEngine {

    class Assets {
    public:
        static int32_t LoadTexture(const std::string& path);
        static int32_t LoadTexture(const std::string& path, const std::string& alias);

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
    };

}
