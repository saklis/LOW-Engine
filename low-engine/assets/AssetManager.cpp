#include "AssetManager.h"

#include <algorithm>

int32_t LowEngine::Assets::AssetManager::LoadTexture(const std::string& path) {
    try {
        sf::Texture texture(path);
        textures.emplace_back(std::move(texture));
        int32_t index = textures.size() - 1;
        return index;
    } catch (sf::Exception ex) {
        _log->error("Failed to load texture: {}", path);
        _log->error("Error: {}", ex.what());
        return -1;
    }
}

int32_t LowEngine::Assets::AssetManager::LoadTexture(const std::string& path, const std::string& alias) {
    int32_t index = LoadTexture(path);
    if (index != -1) {
        textureAliases[alias] = index;
    }
    return index;
}

sf::Texture& LowEngine::Assets::AssetManager::GetTexture(int32_t textureId) {
    if (textures.size() <= textureId) {
        _log->error("Texture with id {} does not exist", textureId);
        throw std::runtime_error("Texture with id does not exist");
    }
    return textures[textureId];
}

sf::Texture& LowEngine::Assets::AssetManager::GetTexture(const std::string& textureAlias) {
    if (textureAliases.find(textureAlias) == textureAliases.end()) {
        _log->error("Texture alias {} does not exist", textureAlias);
        throw std::runtime_error("Texture alias does not exist");
    }
    GetTexture(textureAliases[textureAlias]);
}
