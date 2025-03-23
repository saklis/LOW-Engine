#include "Assets.h"

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

    int Assets::LoadTexture(const std::string& path) {
        try {
            sf::Texture texture(path);
            GetInstance()->_textures.emplace_back(std::move(texture));
            auto index = static_cast<int>(GetInstance()->_textures.size() - 1);
            return index;
        } catch (sf::Exception& ex) {
            _log->error("Failed to load texture: {}", path);
            _log->error("Error: {}", ex.what());
            return -1;
        }
    }

    int Assets::LoadTexture(const std::string& path, const std::string& alias) {
        int index = LoadTexture(path);
        if (index != -1) {
            GetInstance()->_textureAliases[alias] = index;
        }
        return index;
    }

    void Assets::LoadTextureWithAnimationSheet(const std::string& path, unsigned int frameWidth,
                                               unsigned int frameHeight,
                                               unsigned int frameCountX, unsigned int frameCountY) {
        int textureId = LoadTexture(path);
        if (textureId != -1) {
            AddAnimationSheet(textureId, frameWidth, frameHeight, frameCountX, frameCountY);
        }
    }

    void Assets::LoadTextureWithAnimationSheet(const std::string& path, const std::string& alias,
                                               unsigned int frameWidth,
                                               unsigned int frameHeight, unsigned int frameCountX,
                                               unsigned int frameCountY) {
        int textureId = LoadTexture(path, alias);
        if (textureId != -1) {
            AddAnimationSheet(textureId, frameWidth, frameHeight, frameCountX, frameCountY);
        }
    }

    void Assets::AddAnimationSheet(int textureId, unsigned int frameWidth, unsigned int frameHeight,
                                   unsigned int frameCountX,
                                   unsigned int frameCountY) {
        auto it = GetInstance()->_animationSheets.find(textureId);
        if (it == GetInstance()->_animationSheets.end()) {
            Animation::AnimationSheet& sheet = GetInstance()->_animationSheets[textureId];
            sheet.FrameWidth = frameWidth;
            sheet.FrameHeight = frameHeight;
            sheet.FrameCountX = frameCountX;
            sheet.FrameCountY = frameCountY;
        } else {
            _log->error("Texture with id: {} already has an animation sheet.", textureId);
        }
    }

    void Assets::AddAnimationSheet(const std::string& textureAlias, unsigned int frameWidth, unsigned int frameHeight,
                                   unsigned int frameCountX, unsigned int frameCountY) {
        if (GetInstance()->_textureAliases.find(textureAlias) == GetInstance()->_textureAliases.end()) {
            _log->error("Texture alias {} does not exist", textureAlias);
            throw std::runtime_error("Texture alias does not exist");
        }

        AddAnimationSheet(GetInstance()->_textureAliases[textureAlias], frameWidth, frameHeight, frameCountX,
                          frameCountY);
    }

    void Assets::AddAnimationClip(int textureId, const std::string& name, unsigned int firstFrameIndex,
                                  unsigned int frameCount, float frameDuration) {
        auto it = GetInstance()->_animationSheets.find(textureId);
        if (it == GetInstance()->_animationSheets.end()) {
            _log->error("Texture with id {} does not have an animation sheet.", textureId);
            return;
        }
        GetInstance()->_animationSheets[textureId].AddAnimationClip(name, firstFrameIndex, frameCount, frameDuration);
    }

    void Assets::AddAnimationClip(const std::string& textureAlias, const std::string& name,
                                  unsigned int firstFrameIndex,
                                  unsigned int frameCount, float frameDuration) {
        if (GetInstance()->_textureAliases.find(textureAlias) == GetInstance()->_textureAliases.end()) {
            _log->error("Texture alias {} does not exist", textureAlias);
            throw std::runtime_error("Texture alias does not exist");
        }

        AddAnimationClip(GetInstance()->_textureAliases[textureAlias], name, firstFrameIndex, frameCount,
                         frameDuration);
    }

    Animation::AnimationSheet* Assets::GetAnimationSheet(int textureId) {
        auto it = GetInstance()->_animationSheets.find(textureId);
        if (it == GetInstance()->_animationSheets.end()) {
            return nullptr;
        }
        return &it->second;
    }

    Animation::AnimationSheet* Assets::GetAnimationSheet(const std::string& textureAlias) {
        if (GetInstance()->_textureAliases.find(textureAlias) == GetInstance()->_textureAliases.end()) {
            _log->error("Texture alias {} does not exist", textureAlias);
            throw std::runtime_error("Texture alias does not exist");
        }
        return GetAnimationSheet(GetInstance()->_textureAliases[textureAlias]);
    }

    sf::Texture& Assets::GetDefaultTexture() {
        return GetInstance()->_textures[0];
    }

    sf::Texture& Assets::GetTexture(int textureId) {
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
