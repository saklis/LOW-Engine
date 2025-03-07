#include "Assets.h"

#include <algorithm>

namespace LowEngine {
    Assets::Assets() {
        // create default texture
        sf::Image defaultImage;
        defaultImage.resize(sf::Vector2u(32, 32), sf::Color::Magenta);
        sf::Texture defaultTexture;
        if (!defaultTexture.loadFromImage(defaultImage)) {
            _log->error("Failed to load default texture!");
            throw std::runtime_error("Failed to load default texture!");
        }
        _textures.emplace_back(std::move(defaultTexture));
    }

    int32_t Assets::LoadTexture(const std::string& path) {
        try {
            sf::Texture texture(path);
            GetInstance()->_textures.emplace_back(std::move(texture));
            int32_t index = GetInstance()->_textures.size() - 1;
            return index;
        } catch (sf::Exception ex) {
            _log->error("Failed to load texture: {}", path);
            _log->error("Error: {}", ex.what());
            return -1;
        }
    }

    int32_t Assets::LoadTexture(const std::string& path, const std::string& alias) {
        int32_t index = LoadTexture(path);
        if (index != -1) {
            GetInstance()->_textureAliases[alias] = index;
        }
        return index;
    }

    sf::Texture& Assets::GetDefaultTexture() {
        return GetInstance()->_textures[0];
    }

    sf::Texture& Assets::GetTexture(int32_t textureId) {
        if (GetInstance()->_textures.size() <= textureId) {
            _log->error("Texture with id {} does not exist", textureId);
            throw std::runtime_error("Texture with id does not exist");
        }
        return GetInstance()->_textures[textureId];
    }

    sf::Texture& Assets::GetTexture(const std::string& textureAlias) {
        if (GetInstance()->_textureAliases.find(textureAlias) == GetInstance()->_textureAliases.end()) {
            _log->error("Texture alias {} does not exist", textureAlias);
            throw std::runtime_error("Texture alias does not exist");
        }
        return GetTexture(GetInstance()->_textureAliases[textureAlias]);
    }

    void Assets::UnloadAll() {
        GetInstance()->_textures.clear();
        GetInstance()->_textureAliases.clear();
    }
}
