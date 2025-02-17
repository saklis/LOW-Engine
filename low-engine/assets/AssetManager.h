#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "SFML/Graphics/Texture.hpp"
#include "SFML/System/Exception.hpp"

#include "Config.h"

namespace LowEngine::Assets {

    class AssetManager {
    public:
        AssetManager() = default;

        int32_t LoadTexture(const std::string& path);
        int32_t LoadTexture(const std::string& path, const std::string& alias);

        sf::Texture& GetTexture(int32_t textureId);
        sf::Texture& GetTexture(const std::string& textureAlias);

    protected:
        std::vector<sf::Texture> textures;
        std::unordered_map<std::string, int32_t> textureAliases;

        std::shared_ptr<spdlog::logger> _log = spdlog::get(Config::LOGGER_NAME);
    };

}
