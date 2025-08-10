#include "Assets.h"

#include "Config.h"

namespace LowEngine {
    void Assets::CreateDefaultAssets() {
        // create default texture
        if (!_textureAliases.contains(Config::DEFAULT_TEXTURE_ALIAS)) {
            sf::Image defaultImage;
            defaultImage.resize(sf::Vector2u(32, 32), sf::Color::Magenta);
            auto defaultTexture = std::make_unique<Files::Texture>();
            if (!defaultTexture->loadFromImage(defaultImage)) {
                _log->error("Failed to load default texture!");
                throw std::runtime_error("Failed to load default texture!");
            }
            _textures.emplace_back(std::move(defaultTexture));
            _textureAliases[Config::DEFAULT_TEXTURE_ALIAS] = 0;

            _log->debug("Generated default texture with id {}", 0);
        }

        // create default sound
        if (!_soundAliases.contains(Config::DEFAULT_SOUND_ALIAS)) {
            Files::SoundBuffer defaultSound;
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
            _soundAliases[Config::DEFAULT_SOUND_ALIAS] = 0;

            _log->debug("Generated default sound with id {}", 0);
        }

        // create default font
        if (!_fontAliases.contains(Config::DEFAULT_FONT_ALIAS)) {
            sf::Font defaultFont;
            if (!defaultFont.openFromMemory(unitblock_ttf, unitblock_ttf_len)) {
                _log->error("Failed to load default font!");
                throw std::runtime_error("Failed to load default font!");
            }
            _fonts.emplace_back(std::move(defaultFont));
            _fontAliases[Config::DEFAULT_FONT_ALIAS] = 0;

            _log->debug("Generated default font with id {}", _fonts.size() - 1);
        }
    }

    Assets::Assets() {
        CreateDefaultAssets();
    }

    void Assets::LoadDefaultAssets() {
        GetInstance()->CreateDefaultAssets();
    }

    size_t Assets::LoadTexture(const std::string& path) {
        try {
            auto texture = std::make_unique<Files::Texture>(path);
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

    size_t Assets::LoadTexture(const std::string& alias, const std::string& path) {
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
            AddSpriteSheet(textureId, frameCountX, frameCountY);
        }

        return textureId;
    }

    size_t Assets::LoadTextureWithSpriteSheet(const std::string& alias, const std::string& path,
                                              size_t frameWidth,
                                              size_t frameHeight, size_t frameCountX,
                                              size_t frameCountY) {
        size_t textureId = LoadTexture(alias, path);
        if (textureId != Config::MAX_SIZE) {
            AddSpriteSheet(textureId, frameCountX, frameCountY);
        }

        return textureId;
    }

    void Assets::UnloadTexture(size_t textureId) {
        std::string textureAlias = GetTextureAlias(textureId);

        if (!textureAlias.empty()) {
            GetInstance()->_textureAliases.erase(textureAlias);
        }

        GetInstance()->_textures[textureId].reset();
        _log->debug("Texture with id {} and alias '{}' unloaded", textureId, textureAlias);
    }

    void Assets::UnloadTexture(const std::string& textureAlias) {
        auto textureId = GetTextureId(textureAlias);
        UnloadTexture(textureId);
    }

    void Assets::AddSpriteSheet(size_t textureId,
                                size_t frameCountX,
                                size_t frameCountY) {
        auto it = GetInstance()->_animationSheets.find(textureId);
        if (it == GetInstance()->_animationSheets.end()) {
            auto& texture = GetTexture(textureId);
            Animation::SpriteSheet& sheet = GetInstance()->_animationSheets[textureId];
            sheet.FrameCount = sf::Vector2(frameCountX, frameCountY);
            sheet.FrameSize = sf::Vector2(texture.getSize().x / frameCountX, texture.getSize().y / frameCountY);

            _log->debug("Animation sheet added for texture id: {} with frame size: {}x{} and frame count: {}x{}",
                        textureId, sheet.FrameSize.x, sheet.FrameSize.y, sheet.FrameCount.x, sheet.FrameCount.y);
        } else {
            _log->error("Texture with id: {} already has an animation sheet.", textureId);
        }
    }

    void Assets::AddSpriteSheet(const std::string& textureAlias,
                                size_t frameCountX, size_t frameCountY) {
        if (GetInstance()->_textureAliases.find(textureAlias) == GetInstance()->_textureAliases.end()) {
            _log->error("Texture alias {} does not exist", textureAlias);
            throw std::runtime_error("Texture alias does not exist");
        }

        AddSpriteSheet(GetInstance()->_textureAliases[textureAlias], frameCountX, frameCountY);
    }

    void Assets::AddAnimationClip(const std::string& name, size_t textureId, size_t firstFrameIndex,
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

    void Assets::AddAnimationClip(const std::string& name, const std::string& textureAlias,
                                  size_t firstFrameIndex,
                                  size_t frameCount, float frameDuration) {
        if (GetInstance()->_textureAliases.find(textureAlias) == GetInstance()->_textureAliases.end()) {
            _log->error("Texture alias {} does not exist", textureAlias);
            throw std::runtime_error("Texture alias does not exist");
        }

        AddAnimationClip(name, GetInstance()->_textureAliases[textureAlias], firstFrameIndex, frameCount,
                         frameDuration);
    }

    size_t Assets::LoadTileMap(const std::string& path, const std::vector<Terrain::LayerDefinition>& definitions) {
        // get and validate layer definitions
        const Terrain::LayerDefinition* terrainLayerDefinition = nullptr;
        const Terrain::LayerDefinition* featuresLayerDefinition = nullptr;

        for (auto& definition: definitions) {
            switch (definition.Type) {
                case Terrain::LayerType::Terrain:
                    terrainLayerDefinition = &definition;
                    break;
                case Terrain::LayerType::Features:
                    featuresLayerDefinition = &definition;
                    break;
                default:
                    _log->error("Invalid layer definition type: '{}'", ToString(definition.Type));
                    throw std::runtime_error("Invalid layer definition type");
            }
        }

        Terrain::TileMap map(GetDefaultTexture());
        map.LoadFromLDTkJson(path);

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

    size_t Assets::LoadTileMap(const std::string& alias, const std::string& path, const std::vector<Terrain::LayerDefinition>& definitions) {
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

    std::vector<std::string> Assets::GetTileMapAliases() {
        std::vector<std::string> aliases;
        for (const auto& pair: GetInstance()->_mapAliases) {
            aliases.push_back(pair.first);
        }
        return aliases;
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

    bool Assets::TextureExists(const std::string& textureAlias) {
        return GetInstance()->_textureAliases.contains(textureAlias);
    }

    sf::Texture& Assets::GetDefaultTexture() {
        return *GetInstance()->_textures[0];
    }

    Files::Texture& Assets::GetTexture(size_t textureId) {
        if (GetInstance()->_textures.size() <= textureId) {
            _log->error("Texture with id {} does not exist", textureId);
            throw std::runtime_error("Texture with id does not exist");
        }
        return *GetInstance()->_textures[textureId];
    }

    Files::Texture& Assets::GetTexture(const std::string& textureAlias) {
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

    std::vector<std::string> Assets::GetFontAliases() {
        std::vector<std::string> aliases;
        for (const auto& pair: GetInstance()->_fontAliases) {
            aliases.push_back(pair.first);
        }
        return aliases;
    }

    size_t Assets::LoadSound(const std::string& path) {
        try {
            Files::SoundBuffer sound(path);
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

    size_t Assets::LoadSound(const std::string& alias, const std::string& path) {
        size_t index = LoadSound(path);
        if (index != -1) {
            GetInstance()->_soundAliases[alias] = index;
        }

        _log->debug("Sound with id {} loaded with alias '{}'", index, alias);

        return index;
    }

    Files::SoundBuffer& Assets::GetDefaultSound() {
        return GetInstance()->_sounds[0];
    }

    Files::SoundBuffer& Assets::GetSound(size_t soundId) {
        if (GetInstance()->_sounds.size() <= soundId) {
            _log->error("Sound with id {} does not exist", soundId);
            throw std::runtime_error("Sound with id does not exist");
        }

        return GetInstance()->_sounds[soundId];
    }

    Files::SoundBuffer& Assets::GetSound(const std::string& alias) {
        if (GetInstance()->_soundAliases.find(alias) == GetInstance()->_soundAliases.end()) {
            _log->error("Sound alias {} does not exist", alias);
            throw std::runtime_error("Sound alias does not exist");
        }

        return GetSound(GetInstance()->_soundAliases[alias]);
    }

    std::vector<std::string> Assets::GetSoundAliases() {
        std::vector<std::string> aliases;
        for (const auto& pair: GetInstance()->_soundAliases) {
            aliases.push_back(pair.first);
        }
        return aliases;
    }

    nlohmann::ordered_json Assets::SerializeToJSON(const std::filesystem::path& rootDirectory) {
        nlohmann::ordered_json assetsJson;
        nlohmann::ordered_json texturesJson;
        nlohmann::ordered_json spriteSheetsJson;
        nlohmann::ordered_json animationClipsJson;
        nlohmann::ordered_json soundsJson;
        nlohmann::ordered_json fontsJson;
        nlohmann::ordered_json tileMapsJson;

        auto textureAliases = Assets::GetTextureAliases();
        for (const auto& alias: textureAliases) {
            if (alias == Config::DEFAULT_TEXTURE_ALIAS) continue; // skip default texture

            // textures

            auto textureId = Assets::GetTextureId(alias);
            auto texture = Assets::GetTexture(textureId);

            nlohmann::ordered_json textureJson;
            textureJson["alias"] = alias;
            textureJson["path"] = texture.Path.lexically_relative(rootDirectory).generic_string();

            texturesJson.emplace_back(std::move(textureJson));

            // sprite sheets

            auto spriteSheet = Assets::GetSpriteSheet(textureId);
            if (spriteSheet) {
                nlohmann::ordered_json spriteSheetJson;
                spriteSheetJson["textureAlias"] = alias;
                spriteSheetJson["frameWidth"] = spriteSheet->FrameSize.x;
                spriteSheetJson["frameHeight"] = spriteSheet->FrameSize.y;
                spriteSheetJson["frameCountX"] = spriteSheet->FrameCount.x;
                spriteSheetJson["frameCountY"] = spriteSheet->FrameCount.y;

                spriteSheetsJson.emplace_back(std::move(spriteSheetJson));


                // animation clips

                std::vector<std::string> clipNames = spriteSheet->GetAnimationClipNames();

                for (auto& name: clipNames) {
                    auto animClip = spriteSheet->GetAnimationClip(name);

                    nlohmann::ordered_json animationClipJson;
                    animationClipJson["textureAlias"] = alias;
                    animationClipJson["name"] = name;
                    animationClipJson["firstFrameIndex"] = animClip->StartFrame;
                    animationClipJson["frameCount"] = animClip->FrameCount;
                    animationClipJson["frameDuration"] = animClip->FrameDuration;

                    animationClipsJson.emplace_back(std::move(animationClipJson));
                }
            }
        }

        // sounds
        auto soundAliases = Assets::GetSoundAliases();
        for (const auto& alias: soundAliases) {
            if (alias == Config::DEFAULT_SOUND_ALIAS) continue; // skip default sound

            auto sound = Assets::GetSound(alias);
            nlohmann::ordered_json soundJson;
            soundJson["alias"] = alias;
            soundJson["path"] = sound.Path.lexically_relative(rootDirectory).generic_string();;
            soundsJson.emplace_back(std::move(soundJson));
        }

        // fonts
        auto fontAliases = Assets::GetFontAliases();
        for (const auto& alias: fontAliases) {
            if (alias == Config::DEFAULT_FONT_ALIAS) continue; // skip default font

            // TODO: implement font loading first :P
        }

        // tile maps
        std::vector<std::string> tileMapAliases = Assets::GetTileMapAliases();

        for (std::string tileMapAlias: tileMapAliases) {
            auto tileMap = Assets::GetTileMap(tileMapAlias);

            nlohmann::ordered_json tileMapJson;
            tileMapJson["alias"] = tileMapAlias;
            tileMapJson["path"] = tileMap.Path.lexically_relative(rootDirectory).generic_string();;

            nlohmann::ordered_json layerDefinitionsJson;

            nlohmann::ordered_json terrainLayerJson;
            terrainLayerJson["type"] = Terrain::ToString(Terrain::LayerType::Terrain);
            terrainLayerJson["textureAlias"] = Assets::GetTextureAlias(tileMap.TerrainLayer.TextureId);
            nlohmann::ordered_json terrainCellDefinitionsJson;
            for (const auto& cellDef: tileMap.TerrainLayer.Definition.CellDefinitions) {
                nlohmann::ordered_json cellDefJson;
                cellDefJson["id"] = cellDef.first;
                cellDefJson["isWalkable"] = cellDef.second.IsWalkable;
                cellDefJson["isSwimmable"] = cellDef.second.IsSwimmable;
                cellDefJson["isFlyable"] = cellDef.second.IsFlyable;
                cellDefJson["moveCost"] = cellDef.second.MoveCost;
                cellDefJson["animationClipNames"] = cellDef.second.AnimationClipNames;
                terrainCellDefinitionsJson[cellDef.first] = std::move(cellDefJson);
            }
            terrainLayerJson["cellDefinitions"] = terrainCellDefinitionsJson;
            layerDefinitionsJson.emplace_back(std::move(terrainLayerJson));

            nlohmann::ordered_json featuresLayerJson;
            featuresLayerJson["type"] = Terrain::ToString(Terrain::LayerType::Features);
            featuresLayerJson["textureAlias"] = Assets::GetTextureAlias(tileMap.FeaturesLayer.TextureId);
            nlohmann::ordered_json featureCellDefinitionsJson;
            for (const auto& cellDef: tileMap.FeaturesLayer.Definition.CellDefinitions) {
                nlohmann::ordered_json cellDefJson;
                cellDefJson["id"] = cellDef.first;
                cellDefJson["isWalkable"] = cellDef.second.IsWalkable;
                cellDefJson["isSwimmable"] = cellDef.second.IsSwimmable;
                cellDefJson["isFlyable"] = cellDef.second.IsFlyable;
                cellDefJson["moveCost"] = cellDef.second.MoveCost;
                cellDefJson["animationClipNames"] = cellDef.second.AnimationClipNames;
                featureCellDefinitionsJson[cellDef.first] = std::move(cellDefJson);
            }
            featuresLayerJson["cellDefinitions"] = featureCellDefinitionsJson;
            layerDefinitionsJson.emplace_back(std::move(featuresLayerJson));

            tileMapJson["layerDefinitions"] = layerDefinitionsJson;

            tileMapsJson.emplace_back(std::move(tileMapJson));
        }

        assetsJson["textures"] = texturesJson;
        assetsJson["spriteSheets"] = spriteSheetsJson;
        assetsJson["animationClips"] = animationClipsJson;
        assetsJson["sounds"] = soundsJson;
        assetsJson["fonts"] = fontsJson;
        assetsJson["tileMaps"] = tileMapsJson;

        return assetsJson;
    }

    bool Assets::LoadFromJSON(const nlohmann::basic_json<nlohmann::ordered_map>& assetsJson, const std::filesystem::path& assetDirectory) {
        // Load textures
        if (assetsJson.contains("textures")) {
            for (const auto& textureJson: assetsJson["textures"]) {
                if (textureJson.contains("alias") && textureJson.contains("path")) {
                    auto absolutePath = (assetDirectory / std::filesystem::path(textureJson["path"].get<std::string>()).lexically_normal());
                    LoadTexture(textureJson["alias"].get<std::string>(), absolutePath.string());
                } else {
                    _log->error("Invalid texture JSON format");
                    return false;
                }
            }
        }

        // Load sprite sheets
        if (assetsJson.contains("spriteSheets")) {
            for (const auto& spriteSheetJson: assetsJson["spriteSheets"]) {
                if (spriteSheetJson.contains("textureAlias") && spriteSheetJson.contains("frameWidth") &&
                    spriteSheetJson.contains("frameHeight") && spriteSheetJson.contains("frameCountX") &&
                    spriteSheetJson.contains("frameCountY")) {
                    AddSpriteSheet(spriteSheetJson["textureAlias"].get<std::string>(),
                                   spriteSheetJson["frameCountX"].get<size_t>(),
                                   spriteSheetJson["frameCountY"].get<size_t>());
                } else {
                    _log->error("Invalid sprite sheet JSON format");
                    return false;
                }
            }
        }

        // Load animation clips
        if (assetsJson.contains("animationClips")) {
            for (const auto& animationClipJson: assetsJson["animationClips"]) {
                if (animationClipJson.contains("textureAlias") && animationClipJson.contains("name") &&
                    animationClipJson.contains("firstFrameIndex") && animationClipJson.contains("frameCount") &&
                    animationClipJson.contains("frameDuration")) {
                    AddAnimationClip(animationClipJson["name"].get<std::string>(),
                                     animationClipJson["textureAlias"].get<std::string>(),
                                     animationClipJson["firstFrameIndex"].get<size_t>(),
                                     animationClipJson["frameCount"].get<size_t>(),
                                     animationClipJson["frameDuration"].get<float>());
                } else {
                    _log->error("Invalid animation clip JSON format");
                    return false;
                }
            }
        }

        // Load sounds
        if (assetsJson.contains("sounds")) {
            for (const auto& soundJson: assetsJson["sounds"]) {
                if (soundJson.contains("alias") && soundJson.contains("path")) {
                    auto absolutePath = (assetDirectory / std::filesystem::path(soundJson["path"].get<std::string>()).lexically_normal());
                    LoadSound(soundJson["alias"].get<std::string>(), absolutePath.string());
                } else {
                    _log->error("Invalid sound JSON format");
                    return false;
                }
            }
        }

        // Load fonts
        // TODO: Implement font loading from JSON

        // Load tile maps
        if (assetsJson.contains("tileMaps")) {
            for (const auto& tileMapJson: assetsJson["tileMaps"]) {
                if (tileMapJson.contains("alias") && tileMapJson.contains("path") &&
                    tileMapJson.contains("layerDefinitions")) {
                    std::vector<Terrain::LayerDefinition> layerDefinitions;
                    for (const auto& layerDefJson: tileMapJson["layerDefinitions"]) {
                        Terrain::LayerDefinition layerDef;
                        layerDef.Type = Terrain::FromString(layerDefJson["type"].get<std::string>());
                        layerDef.TextureId = GetTextureId(layerDefJson["textureAlias"].get<std::string>());
                        for (const auto& cellDefJson: layerDefJson["cellDefinitions"].items()) {
                            Terrain::CellDefinition cellDef;
                            cellDef.IsWalkable = cellDefJson.value().at("isWalkable").get<bool>();
                            cellDef.IsSwimmable = cellDefJson.value().at("isSwimmable").get<bool>();
                            cellDef.IsFlyable = cellDefJson.value().at("isFlyable").get<bool>();
                            cellDef.MoveCost = cellDefJson.value().at("moveCost").get<float>();
                            if (cellDefJson.value().contains("animationClipNames")) {
                                for (const auto& clipName: cellDefJson.value().at("animationClipNames")) {
                                    cellDef.AnimationClipNames.push_back(clipName.get<std::string>());
                                }
                            }
                            layerDef.CellDefinitions[cellDefJson.value().at("id").get<unsigned>()] = std::move(cellDef);
                        }
                        layerDefinitions.push_back(std::move(layerDef));
                    }
                    auto absolutePath = (assetDirectory / std::filesystem::path(tileMapJson["path"].get<std::string>()).lexically_normal());
                    LoadTileMap(tileMapJson["alias"].get<std::string>(), absolutePath.string(), layerDefinitions);
                } else {
                    _log->error("Invalid tile map JSON format");
                    return false;
                }
            }
        }

        return true;
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

        map.TerrainLayer.Definition = *terrainLayerDefinition;
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

        map.FeaturesLayer.Definition = *featuresLayerDefinition;
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
