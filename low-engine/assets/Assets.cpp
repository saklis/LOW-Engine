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
        _textureAliases["default"] = 0;

        _log->debug("Generated default texture with id {}", 0);

        // create default sound
        sf::SoundBuffer defaultSound;
        const unsigned int sampleRate = 44100;
        const float duration = 1.f;
        const auto sampleCount = static_cast<size_t>(sampleRate * duration);
        std::vector<short> samples(sampleCount);

        // Generate a 440 Hz sine wave tone
        const double amplitude = 30000;
        const double frequency = 440.0;
        const double twoPiF = 2 * 3.14159 * frequency;

        for (size_t i = 0; i < sampleCount; ++i) {
            samples[i] = static_cast<short>(amplitude * std::sin(twoPiF * i / sampleRate));
        }

        std::vector<sf::SoundChannel> channelMap{sf::SoundChannel::Mono};

        if (!defaultSound.loadFromSamples(samples.data(), samples.size(), 1, sampleRate, channelMap)) {
            _log->error("Failed to load default sound!");
            throw std::runtime_error("Failed to load default sound!");
        }
        _sounds.emplace_back(std::move(defaultSound));
        _soundAliases["default"] = 0;

        _log->debug("Generated default sound with id {}", 0);

        // create default font
        sf::Font defaultFont;
        if (!defaultFont.openFromMemory(unitblock_ttf, unitblock_ttf_len)) {
            _log->error("Failed to load default font!");
            throw std::runtime_error("Failed to load default font!");
        }
        _fonts.emplace_back(std::move(defaultFont));

        _log->debug("Generated default font with id {}", _fonts.size() - 1);
    }

    size_t Assets::LoadTexture(const std::string& path) {
        try {
            sf::Texture texture(path);
            GetInstance()->_textures.emplace_back(std::move(texture));
            size_t index = static_cast<int>(GetInstance()->_textures.size() - 1);

            _log->debug("New texture loaded: {} with id {}", path, index);

            return index;
        } catch (sf::Exception& ex) {
            _log->error("Failed to load texture: {}", path);
            _log->error("Error: {}", ex.what());
            return Config::MAX_SIZE;
        }
    }

    size_t Assets::LoadTexture(const std::string& path, const std::string& alias) {
        size_t index = LoadTexture(path);
        if (index != Config::MAX_SIZE) {
            GetInstance()->_textureAliases[alias] = index;
        }

        _log->debug("Texture with id {} loaded with alias '{}'", index, alias);

        return index;
    }

    size_t Assets::LoadTextureWithSpriteSheet(const std::string& path, size_t frameWidth,
                                              size_t frameHeight,
                                              size_t frameCountX, size_t frameCountY) {
        size_t textureId = LoadTexture(path);
        if (textureId != Config::MAX_SIZE) {
            AddSpriteSheet(textureId, frameWidth, frameHeight, frameCountX, frameCountY);
        }

        return textureId;
    }

    size_t Assets::LoadTextureWithSpriteSheet(const std::string& path, const std::string& alias,
                                              size_t frameWidth,
                                              size_t frameHeight, size_t frameCountX,
                                              size_t frameCountY) {
        size_t textureId = LoadTexture(path, alias);
        if (textureId != Config::MAX_SIZE) {
            AddSpriteSheet(textureId, frameWidth, frameHeight, frameCountX, frameCountY);
        }

        return textureId;
    }

    void Assets::AddSpriteSheet(size_t textureId, size_t frameWidth, size_t frameHeight,
                                size_t frameCountX,
                                size_t frameCountY) {
        auto it = GetInstance()->_animationSheets.find(textureId);
        if (it == GetInstance()->_animationSheets.end()) {
            Animation::SpriteSheet& sheet = GetInstance()->_animationSheets[textureId];
            sheet.FrameSize = sf::Vector2(frameWidth, frameHeight);
            sheet.FrameCount = sf::Vector2(frameCountX, frameCountY);

            _log->debug("Animation sheet added for texture id: {} with frame size: {}x{} and frame count: {}x{}",
                        textureId, frameWidth, frameHeight, frameCountX, frameCountY);
        } else {
            _log->error("Texture with id: {} already has an animation sheet.", textureId);
        }
    }

    void Assets::AddSpriteSheet(const std::string& textureAlias, size_t frameWidth, size_t frameHeight,
                                size_t frameCountX, size_t frameCountY) {
        if (GetInstance()->_textureAliases.find(textureAlias) == GetInstance()->_textureAliases.end()) {
            _log->error("Texture alias {} does not exist", textureAlias);
            throw std::runtime_error("Texture alias does not exist");
        }

        AddSpriteSheet(GetInstance()->_textureAliases[textureAlias], frameWidth, frameHeight, frameCountX,
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
        firstFrameOrigin.x = firstFrameIndex % animSheet->second.FrameCount.x * animSheet->second.FrameSize.x;
        firstFrameOrigin.y = firstFrameIndex / animSheet->second.FrameCount.x * animSheet->second.FrameSize.y;
        GetInstance()->_animationSheets[textureId].AddAnimationClip(name, firstFrameIndex, frameCount, frameDuration, firstFrameOrigin);

        _log->debug("Animation clip added for texture id: {} with name: '{}' and frame count: {}",
                    textureId, name, frameCount);
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

    size_t Assets::LoadTileMap(const std::string& path, const std::vector<Terrain::LayerDefinition>& definitions) {
        std::ifstream file(path);
        if (!file.is_open()) {
            _log->error("Failed to load terrain file: {}", path);
            throw std::runtime_error("Failed to load terrain");
        }

        // get layer definitions
        const Terrain::LayerDefinition* terrainLayerDefinition = nullptr;
        const Terrain::LayerDefinition* featuresLayerDefinition = nullptr;

        for (auto& definition: definitions) {
            switch (definition.Type) {
                case Terrain::Terrain:
                    terrainLayerDefinition = &definition;
                    break;
                case Terrain::Features:
                    featuresLayerDefinition = &definition;
                    break;
                default:
                    _log->error("Invalid layer definition type: '{}'", LayerTypeToString(definition.Type));
                    throw std::runtime_error("Invalid layer definition type");
            }
        }

        nlohmann::json jsonData;
        file >> jsonData;

        Terrain::TileMap map(GetDefaultTexture());
        map.LoadFromLDTkJson(jsonData);

        if (terrainLayerDefinition != nullptr) {
            LoadTerrainLayerData(terrainLayerDefinition, map);
            ReadNavDataForLayer(map, map.TerrainLayer, terrainLayerDefinition);
        }
        if (featuresLayerDefinition != nullptr) {
            LoadFeatureLayerData(featuresLayerDefinition, map);
            ReadNavDataForLayer(map, map.FeaturesLayer, featuresLayerDefinition);
        }

        GetInstance()->_maps.emplace_back(std::move(map));
        size_t index = static_cast<int>(GetInstance()->_maps.size() - 1);

        _log->debug("New map loaded: {} with id {}", path, index);

        return index;
    }

    size_t Assets::LoadTileMap(const std::string& path, const std::string& alias, const std::vector<Terrain::LayerDefinition>& definitions) {
        size_t index = LoadTileMap(path, definitions);
        if (index != -1) {
            GetInstance()->_mapAliases[alias] = index;
        }
        return index;
    }

    Terrain::TileMap& Assets::GetTileMap(size_t mapId) {
        if (GetInstance()->_maps.size() <= mapId) {
            _log->error("Map with id {} does not exist", mapId);
            throw std::runtime_error("Map with id does not exist");
        }

        return GetInstance()->_maps[mapId];
    }

    Terrain::TileMap& Assets::GetTileMap(const std::string& mapAlias) {
        auto map = GetInstance()->_mapAliases.find(mapAlias);
        if (map == GetInstance()->_mapAliases.end()) {
            _log->error("Map with alias {} does not exist", mapAlias);
            throw std::runtime_error("Map with alias does not exist");
        }

        return GetInstance()->_maps[map->second];
    }

    size_t Assets::GetTileMapId(const std::string& mapAlias) {
        auto map = GetInstance()->_mapAliases.find(mapAlias);
        if (map == GetInstance()->_mapAliases.end()) {
            _log->error("Map with alias {} does not exist", mapAlias);
            throw std::runtime_error("Map with alias does not exist");
        }
        return map->second;
    }

    Animation::SpriteSheet* Assets::GetSpriteSheet(size_t textureId) {
        auto it = GetInstance()->_animationSheets.find(textureId);
        if (it == GetInstance()->_animationSheets.end()) {
            return nullptr;
        }
        return &it->second;
    }

    Animation::SpriteSheet* Assets::GetSpriteSheet(const std::string& textureAlias) {
        if (GetInstance()->_textureAliases.find(textureAlias) == GetInstance()->_textureAliases.end()) {
            _log->error("Texture alias {} does not exist", textureAlias);
            throw std::runtime_error("Texture alias does not exist");
        }
        return GetSpriteSheet(GetInstance()->_textureAliases[textureAlias]);
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

    std::string Assets::GetTextureAlias(size_t textureId) {
        for (auto& pair: GetInstance()->_textureAliases) {
            if (pair.second == textureId) {
                return pair.first;
            }
        }

        _log->error("Texture with id {} does not have an alias", textureId);
        throw std::runtime_error("Texture with id does not have an alias");
    }

    std::vector<std::string> Assets::GetTextureAliases() {
        std::vector<std::string> aliases;
        for (const auto& pair: GetInstance()->_textureAliases) {
            aliases.push_back(pair.first);
        }
        return aliases;
    }

    sf::Font& Assets::GetDefaultFont() {
        return GetInstance()->_fonts[0];
    }

    size_t Assets::LoadSound(const std::string& path) {
        try {
            sf::SoundBuffer sound(path);
            GetInstance()->_sounds.emplace_back(std::move(sound));
            size_t index = static_cast<int>(GetInstance()->_sounds.size() - 1);

            _log->debug("New sound loaded: {} with id {}", path, index);

            return index;
        } catch (sf::Exception& ex) {
            _log->error("Failed to load sound: {}", path);
            _log->error("Error: {}", ex.what());
            return -1;
        }
    }

    size_t Assets::LoadSound(const std::string& path, const std::string& alias) {
        size_t index = LoadSound(path);
        if (index != -1) {
            GetInstance()->_soundAliases[alias] = index;
        }

        _log->debug("Sound with id {} loaded with alias '{}'", index, alias);

        return index;
    }

    sf::SoundBuffer& Assets::GetDefaultSound() {
        return GetInstance()->_sounds[0];
    }

    sf::SoundBuffer& Assets::GetSound(size_t soundId) {
        if (GetInstance()->_sounds.size() <= soundId) {
            _log->error("Sound with id {} does not exist", soundId);
            throw std::runtime_error("Sound with id does not exist");
        }

        return GetInstance()->_sounds[soundId];
    }

    sf::SoundBuffer& Assets::GetSound(const std::string& soundAlias) {
        if (GetInstance()->_soundAliases.find(soundAlias) == GetInstance()->_soundAliases.end()) {
            _log->error("Sound alias {} does not exist", soundAlias);
            throw std::runtime_error("Sound alias does not exist");
        }

        return GetSound(GetInstance()->_soundAliases[soundAlias]);
    }

    void Assets::UnloadAll() {
        GetInstance()->_maps.clear();
        GetInstance()->_mapAliases.clear();

        GetInstance()->_textures.clear();
        GetInstance()->_textureAliases.clear();
        GetInstance()->_animationSheets.clear();

        GetInstance()->_fonts.clear();
        GetInstance()->_fontAliases.clear();

        GetInstance()->_sounds.clear();
        GetInstance()->_soundAliases.clear();

        _log->info("All assets unloaded");
    }

    void Assets::LoadTerrainLayerData(const Terrain::LayerDefinition* terrainLayerDefinition, Terrain::TileMap& map) {
        std::random_device rd;
        std::mt19937 gen(rd());

        size_t textureId = terrainLayerDefinition->TextureId;

        map.TerrainLayer.LoadTexture(textureId);
        auto animSheet = GetSpriteSheet(textureId);
        if (animSheet == nullptr) {
            _log->error("Sprite sheet does not exist for texture id {}", textureId);
            throw std::runtime_error("Sprite sheet does not exist");
        }

        for (auto& cellDefinition: terrainLayerDefinition->CellDefinitions) {
            for (auto animClipName: cellDefinition.second.AnimationClipNames) {
                auto clip = animSheet->GetAnimationClip(animClipName);
                if (clip == nullptr) {
                    _log->error("Animation clip {} does not exist for texture id {}", animClipName, textureId);
                    throw std::runtime_error("Animation clip does not exist");
                }
                map.TerrainLayer.AnimatedTiles[cellDefinition.first].Clips.emplace_back(clip);
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

    void Assets::LoadFeatureLayerData(const Terrain::LayerDefinition* featuresLayerDefinition, Terrain::TileMap& map) {
        std::random_device rd;
        std::mt19937 gen(rd());

        size_t textureId = featuresLayerDefinition->TextureId;

        map.FeaturesLayer.LoadTexture(textureId);
        auto animSheet = GetSpriteSheet(textureId);
        if (animSheet == nullptr) {
            _log->error("Sprite sheet does not exist for texture id {}", textureId);
            throw std::runtime_error("Sprite sheet does not exist");
        }

        for (auto& cellDefinition: featuresLayerDefinition->CellDefinitions) {
            for (auto animClipName: cellDefinition.second.AnimationClipNames) {
                auto clip = animSheet->GetAnimationClip(animClipName);
                if (clip == nullptr) {
                    _log->error("Animation clip {} does not exist for texture id {}", animClipName, textureId);
                    throw std::runtime_error("Animation clip does not exist");
                }
                map.FeaturesLayer.AnimatedTiles[cellDefinition.first].Clips.emplace_back(clip);
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

    void Assets::ReadNavDataForLayer(Terrain::TileMap& map, Terrain::Layer& layer, const Terrain::LayerDefinition* layerDefinition) {
        for (size_t i = 0; i < map.NavGrid.Cells.size(); i++) {
            auto& navCell = map.NavGrid.Cells[i];
            navCell.Position.x = i % map.NavGrid.Width;
            navCell.Position.y = i / map.NavGrid.Width;

            size_t terrainCellIndexType = layer.Cells[i];
            if (terrainCellIndexType != Config::MAX_SIZE) {
                auto& typeDefinition = layerDefinition->CellDefinitions.at(terrainCellIndexType);

                navCell.IsWalkable = typeDefinition.IsWalkable;
                navCell.IsSwimmable = typeDefinition.IsSwimmable;
                navCell.IsFlyable = typeDefinition.IsFlyable;
                navCell.MoveCost = typeDefinition.MoveCost;
            }
        }
    }
}
