#include "Assets.h"

#include "Config.h"

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

        // create default texture alias
        _textureAliases["default"] = 0;

        // create default font
        sf::Font defaultFont;
        if (!defaultFont.openFromMemory(unitblock_ttf, unitblock_ttf_len)) {
            _log->error("Failed to load default font!");
            throw std::runtime_error("Failed to load default font!");
        }
        _fonts.emplace_back(std::move(defaultFont));
    }

    size_t Assets::LoadTexture(const std::string& path) {
        try {
            sf::Texture texture(path);
            GetInstance()->_textures.emplace_back(std::move(texture));
            size_t index = static_cast<int>(GetInstance()->_textures.size() - 1);
            return index;
        } catch (sf::Exception& ex) {
            _log->error("Failed to load texture: {}", path);
            _log->error("Error: {}", ex.what());
            return -1;
        }
    }

    size_t Assets::LoadTexture(const std::string& path, const std::string& alias) {
        size_t index = LoadTexture(path);
        if (index != -1) {
            GetInstance()->_textureAliases[alias] = index;
        }
        return index;
    }

    void Assets::LoadTextureWithAnimationSheet(const std::string& path, size_t frameWidth,
                                               size_t frameHeight,
                                               size_t frameCountX, size_t frameCountY) {
        size_t textureId = LoadTexture(path);
        if (textureId != -1) {
            AddAnimationSheet(textureId, frameWidth, frameHeight, frameCountX, frameCountY);
        }
    }

    void Assets::LoadTextureWithAnimationSheet(const std::string& path, const std::string& alias,
                                               size_t frameWidth,
                                               size_t frameHeight, size_t frameCountX,
                                               size_t frameCountY) {
        size_t textureId = LoadTexture(path, alias);
        if (textureId != -1) {
            AddAnimationSheet(textureId, frameWidth, frameHeight, frameCountX, frameCountY);
        }
    }

    void Assets::AddAnimationSheet(size_t textureId, size_t frameWidth, size_t frameHeight,
                                   size_t frameCountX,
                                   size_t frameCountY) {
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

    void Assets::AddAnimationSheet(const std::string& textureAlias, size_t frameWidth, size_t frameHeight,
                                   size_t frameCountX, size_t frameCountY) {
        if (GetInstance()->_textureAliases.find(textureAlias) == GetInstance()->_textureAliases.end()) {
            _log->error("Texture alias {} does not exist", textureAlias);
            throw std::runtime_error("Texture alias does not exist");
        }

        AddAnimationSheet(GetInstance()->_textureAliases[textureAlias], frameWidth, frameHeight, frameCountX,
                          frameCountY);
    }

    void Assets::AddAnimationClip(size_t textureId, const std::string& name, size_t firstFrameIndex,
                                  size_t frameCount, float frameDuration) {
        auto animSheet = GetInstance()->_animationSheets.find(textureId);
        if (animSheet == GetInstance()->_animationSheets.end()) {
            _log->error("Texture with id {} does not have an animation sheet.", textureId);
            return;
        }
        sf::Vector2<size_t> firstFrameOrigin;
        firstFrameOrigin.x = firstFrameIndex % animSheet->second.FrameCountX * animSheet->second.FrameWidth;
        firstFrameOrigin.y = firstFrameIndex / animSheet->second.FrameCountX * animSheet->second.FrameHeight;
        GetInstance()->_animationSheets[textureId].AddAnimationClip(name, firstFrameIndex, frameCount, frameDuration, firstFrameOrigin);
    }

    void Assets::AddAnimationClip(const std::string& textureAlias, const std::string& name,
                                  size_t firstFrameIndex,
                                  size_t frameCount, float frameDuration) {
        if (GetInstance()->_textureAliases.find(textureAlias) == GetInstance()->_textureAliases.end()) {
            _log->error("Texture alias {} does not exist", textureAlias);
            throw std::runtime_error("Texture alias does not exist");
        }

        AddAnimationClip(GetInstance()->_textureAliases[textureAlias], name, firstFrameIndex, frameCount,
                         frameDuration);
    }

    size_t Assets::LoadMap(const std::string& path, const std::vector<Terrain::LayerToTextureMapping>& mappings) {
        std::ifstream file(path);
        if (!file.is_open()) {
            _log->error("Failed to load terrain: {}", path);
            throw std::runtime_error("Failed to load terrain");
        }

        nlohmann::json jsonData;
        file >> jsonData;

        Terrain::Map map(GetDefaultTexture());
        map.LoadFromLDTkJson(jsonData);

        std::random_device rd;
        std::mt19937 gen(rd());

        for (auto& mapping: mappings) {
            switch (mapping.Type) {
                case Terrain::Terrain: {
                    map.TerrainLayer.LoadTexture(mapping.TextureId);

                    auto animSheet = GetAnimationSheet(mapping.TextureId);
                    for (size_t clipNameIndex = 0; clipNameIndex < mapping.AnimationClipNames.size(); clipNameIndex++) {
                        auto clipNames = mapping.AnimationClipNames[clipNameIndex];
                        if (!clipNames.empty()) {
                            if (animSheet == nullptr) {
                                _log->error("Animation sheet does not exist for texture id {}", mapping.TextureId);
                                throw std::runtime_error("Animation sheet does not exist");
                            }
                            for (const auto& clipName: clipNames) {
                                auto clip = animSheet->GetAnimationClip(clipName);
                                if (clip == nullptr) {
                                    _log->error("Animation clip {} does not exist for texture id {}", clipName, mapping.TextureId);
                                    throw std::runtime_error("Animation clip does not exist");
                                }
                                map.TerrainLayer.AnimatedTiles[clipNameIndex].Clips.emplace_back(clip);
                            }
                        }
                    }

                    for (size_t cellIndex = 0; cellIndex < map.TerrainLayer.Cells.size(); cellIndex++) {
                        auto cellValue = map.TerrainLayer.Cells[cellIndex];
                        if (cellValue != Config::MAX_SIZE) {
                            if (map.TerrainLayer.AnimatedTiles.size() > 1) {
                                if (map.TerrainLayer.AnimatedTiles.find(cellValue) != map.TerrainLayer.AnimatedTiles.end()) {
                                    std::uniform_int_distribution<> dis(0, static_cast<int>(map.TerrainLayer.AnimatedTiles.size() - 1));
                                    size_t clipIndex = dis(gen);
                                    map.TerrainLayer.CellClipIndex[cellIndex] = clipIndex;
                                }
                            } else {
                                map.TerrainLayer.CellClipIndex[cellIndex] = 0;
                            }
                        }
                    }
                }
                break;

                case Terrain::Features: {
                    map.FeaturesLayer.LoadTexture(mapping.TextureId);

                    auto animSheet = GetAnimationSheet(mapping.TextureId);
                    for (size_t clipNameIndex = 0; clipNameIndex < mapping.AnimationClipNames.size(); clipNameIndex++) {
                        auto clipNames = mapping.AnimationClipNames[clipNameIndex];
                        if (!clipNames.empty()) {
                            if (animSheet == nullptr) {
                                _log->error("Animation sheet does not exist for texture id {}", mapping.TextureId);
                                throw std::runtime_error("Animation sheet does not exist");
                            }
                            for (const auto& clipName: clipNames) {
                                auto clip = animSheet->GetAnimationClip(clipName);
                                if (clip == nullptr) {
                                    _log->error("Animation clip {} does not exist for texture id {}", clipName, mapping.TextureId);
                                    throw std::runtime_error("Animation clip does not exist");
                                }
                                map.FeaturesLayer.AnimatedTiles[clipNameIndex].Clips.emplace_back(clip);
                            }
                        }
                    }

                    for (size_t cellIndex = 0; cellIndex < map.FeaturesLayer.Cells.size(); cellIndex++) {
                        auto cellValue = map.FeaturesLayer.Cells[cellIndex];
                        if (cellValue != Config::MAX_SIZE) {
                            auto animTile = map.FeaturesLayer.AnimatedTiles.find(cellValue);
                            if (animTile != map.FeaturesLayer.AnimatedTiles.end()) {
                                if (animTile->second.Clips.size() >= 2) {
                                    std::uniform_int_distribution<> dis(0, static_cast<int>(animTile->second.Clips.size() - 1));
                                    size_t clipIndex = dis(gen);
                                    map.FeaturesLayer.CellClipIndex[cellIndex] = clipIndex;
                                } else {
                                    map.FeaturesLayer.CellClipIndex[cellIndex] = 0;
                                }
                            }
                        }
                    }
                }
                break;
            }
        }

        GetInstance()->_maps.emplace_back(std::move(map));
        size_t index = static_cast<int>(GetInstance()->_maps.size() - 1);
        return index;
    }

    size_t Assets::LoadMap(const std::string& path, const std::string& alias, const std::vector<Terrain::LayerToTextureMapping>& mappings) {
        size_t index = LoadMap(path, mappings);
        if (index != -1) {
            GetInstance()->_mapAliases[alias] = index;
        }
        return index;
    }

    Terrain::Map& Assets::GetMap(size_t mapId) {
        if (GetInstance()->_maps.size() <= mapId) {
            _log->error("Map with id {} does not exist", mapId);
            throw std::runtime_error("Map with id does not exist");
        }

        return GetInstance()->_maps[mapId];
    }

    Animation::AnimationSheet* Assets::GetAnimationSheet(size_t textureId) {
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

    sf::Texture& Assets::GetTexture(size_t textureId) {
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

    size_t Assets::GetTextureId(const std::string& textureAlias) {
        if (GetInstance()->_textureAliases.find(textureAlias) == GetInstance()->_textureAliases.end()) {
            _log->error("Texture alias {} does not exist", textureAlias);
            throw std::runtime_error("Texture alias does not exist");
        }
        return GetInstance()->_textureAliases[textureAlias];
    }

    sf::Font& Assets::GetDefaultFont() {
        return GetInstance()->_fonts[0];
    }

    void Assets::UnloadAll() {
        GetInstance()->_maps.clear();
        GetInstance()->_mapAliases.clear();

        GetInstance()->_textures.clear();
        GetInstance()->_textureAliases.clear();
        GetInstance()->_animationSheets.clear();

        GetInstance()->_fonts.clear();
        GetInstance()->_fontAliases.clear();
    }
}
