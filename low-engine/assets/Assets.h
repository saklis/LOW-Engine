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

#include "animation/SpriteSheet.h"
#include "terrain/TileMap.h"

#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Sprite.hpp"

#include "defaults/unitblock.hpp"
#include "SFML/Audio/SoundBuffer.hpp"
#include "terrain/LayerDefinition.h"

namespace LowEngine {
    /**
     * @brief Asset Manager for LowEngine.
     *
     * Static (singleton) class that manages loading and accessing textures, sounds, fonts, and other assets.
     */
    class Assets {
    public:
        /**
         * @brief Load texture from file
         * @param path Path to the texture file
         * @return Texture ID
         */
        static size_t LoadTexture(const std::string& path);

        /**
         * @brief Load texture from file with alias
         * @param path Path to texture file
         * @param alias Alias that will be used to access the texture
         * @return Unique ID of loaded texture
         */
        static size_t LoadTexture(const std::string& path, const std::string& alias);

        /**
         * @brief Load texture from file and create animation sheet for it.
         * @param path Path to the texture file
         * @param frameWidth Width of a single frame in pixels
         * @param frameHeight Height of a single frame in pixels
         * @param frameCountX Number of frames in X direction
         * @param frameCountY Number of frames in Y direction
         * @return Unique ID of loaded texture
         */
        static size_t LoadTextureWithSpriteSheet(const std::string& path, size_t frameWidth,
                                                    size_t frameHeight, size_t frameCountX,
                                                    size_t frameCountY);

        /**
         * @brief Load texture from file with alias and create animation sheet for it.
         * @param path Path to the texture file
         * @param alias Alias that will be used to access the texture
         * @param frameWidth Width of a single frame in pixels
         * @param frameHeight Height of a single frame in pixels
         * @param frameCountX Number of frames in X direction
         * @param frameCountY Number of frames in Y direction
         * @return Unique ID of loaded texture
         */
        static size_t LoadTextureWithSpriteSheet(const std::string& path, const std::string& alias,
                                                    size_t frameWidth, size_t frameHeight,
                                                    size_t frameCountX, size_t frameCountY);

        /**
         * @brief Add animation sheet for texture
         * @param textureId ID of the texture
         * @param frameWidth Width of a single frame in pixels
         * @param frameHeight Height of a single frame in pixels
         * @param frameCountX Number of frames in X direction
         * @param frameCountY Number of frames in Y direction
         */
        static void AddSpriteSheet(size_t textureId, size_t frameWidth, size_t frameHeight,
                                      size_t frameCountX, size_t frameCountY);

        /**
         * @brief Add animation sheet for texture with alias
         * @param textureAlias Texture alias
         * @param frameWidth Width of a single frame in pixels
         * @param frameHeight Height of a single frame in pixels
         * @param frameCountX Number of frames in X direction
         * @param frameCountY Number of frames in Y direction
         */
        static void AddSpriteSheet(const std::string& textureAlias, size_t frameWidth,
                                      size_t frameHeight, size_t frameCountX, size_t frameCountY);

        /**
         * @brief Define Animation Clip for a texture.
         *
         * Texture need to have an animation sheet defined before.
         * @param textureId ID of the texture
         * @param name Name of the animation clip
         * @param firstFrameIndex Index of the first frame in the animation
         * @param frameCount Number of frames in the animation
         * @param frameDuration Duration of a single frame in seconds
         */
        static void AddAnimationClip(size_t textureId, const std::string& name, size_t firstFrameIndex,
                                     size_t frameCount, float frameDuration);

        /**
         * @brief Define Animation Clip for a texture with alias.
         *
         * Texture need to have an animation sheet defined before.
         * @param textureAlias Texture alias
         * @param name Name of the animation clip
         * @param firstFrameIndex Index of the first frame in the animation
         * @param frameCount Number of frames in the animation
         * @param frameDuration Duration of a single frame in seconds
         */
        static void AddAnimationClip(const std::string& textureAlias, const std::string& name,
                                     size_t firstFrameIndex, size_t frameCount, float frameDuration);

        /**
         * @brief Load tile map from file.
         *
         * Supports LDTk format (*.ldtkl). Names of layers in file MUST follow the same naming convention as in LowEngine::Terrain::LayerType enum.
         *
         * Mappings are used to assign texture to layer. Only texture following specified layout are supported.
         * Texture must be a vertical texture atlas with all tiles in a single column. Any additional columns can be used for animated tiles.
         * When loading map, engine will read tile's y coordinate as index that later will be used to find tile on the texture.
         *
         * For animated tiles, Animation Clips with aliases must be defined before loading map.
         * Aliases for Clips must be provided on appropriate indexes of LayerToTextureMapping::AnimationClipNames vector.
         * Multiple clips can be defined for a single tile - in that case particular tile with have Clip assigned randomly.
         * @param path Path to the map file (*.ldtkl)
         * @param definitions Vector for each layer to map texture and Animations Clips.
         * @return Map ID
         */
        static size_t LoadTileMap(const std::string& path, const std::vector<Terrain::LayerDefinition>& definitions);

        /**
         * @brief Load tile map from file with alias.
         *
         * Supports LDTk format (*.ldtkl). Names of layers in file MUST follow the same naming convention as in LowEngine::Terrain::LayerType enum.
         *
         * Mappings are used to assign texture to layer. Only texture following specified layout are supported.
         * Texture must be a vertical texture atlas with all tiles in a single column. Any additional columns can be used for animated tiles.
         * When loading map, engine will read tile's y coordinate as index that later will be used to find tile on the texture.
         *
         * For animated tiles, Animation Clips with aliases must be defined before loading map.
         * Aliases for Clips must be provided on appropriate indexes of LayerToTextureMapping::AnimationClipNames vector.
         * Multiple clips can be defined for a single tile - in that case particular tile with have Clip assigned randomly.
         * @param path Path to the map file (*.ldtkl)
         * @param alias Alias that will be used to access the map
         * @param definitions Vector for each layer to map texture and Animations Clips.
         * @return Map ID
         */
        static size_t LoadTileMap(const std::string& path, const std::string& alias, const std::vector<Terrain::LayerDefinition>& definitions);

        /**
         * @brief Retrieve a tile map by its ID.
         * @param mapId The unique ID of the tile map to retrieve.
         * @return Reference to the requested `Terrain::TileMap`.
         */
        static Terrain::TileMap& GetTileMap(size_t mapId);

        /**
         * @brief Retrieve a tile map by its alias.
         * @param mapAlias The alias of the tile map to retrieve.
         * @return Reference to the requested `Terrain::TileMap`.
         */
        static Terrain::TileMap& GetTileMap(const std::string& mapAlias);

        /**
         * @brief Retrieve Id of the map by its alias.
         * @param mapAlias Alias of the map.
         * @return Id of the map.
         */
        static size_t GetTileMapId(const std::string& mapAlias);

        /**
         * @brief Retrieve the animation sheet associated with a texture ID.
         * @param textureId The unique ID of the texture.
         * @return Pointer to the `Animation::AnimationSheet` if it exists, otherwise `nullptr`.
         */
        static Animation::SpriteSheet* GetSpriteSheet(size_t textureId);

        /**
         * @brief Retrieve the animation sheet associated with a texture alias.
         * @param textureAlias The alias of the texture.
         * @return Pointer to the `Animation::AnimationSheet` if it exists, otherwise throws an exception.
         */
        static Animation::SpriteSheet* GetSpriteSheet(const std::string& textureAlias);

        /**
         * @brief Retrieve the default texture.
         * @return Reference to the default `sf::Texture`.
         */
        static sf::Texture& GetDefaultTexture();

        /**
         * @brief Retrieve a texture by its ID.
         * @param textureId The unique ID of the texture to retrieve.
         * @return Reference to the requested `sf::Texture`.
         */
        static sf::Texture& GetTexture(size_t textureId);

        /**
         * @brief Retrieve a texture by its alias.
         * @param textureAlias The alias of the texture to retrieve.
         * @return Reference to the requested `sf::Texture`.
         */
        static sf::Texture& GetTexture(const std::string& textureAlias);

        /**
         * @brief Retrieve the ID of a texture by its alias.
         * @param textureAlias The alias of the texture.
         * @return The unique ID of the requested texture.
         */
        static size_t GetTextureId(const std::string& textureAlias);

        /**
         * @brief Retrive the default font.
         * @return Reference to the default font.
         */
        static sf::Font& GetDefaultFont();

        /**
         * @brief Load sound from file.
         * @param path Path to the sound file
         * @return Unique ID of loaded sound
         */
        static size_t LoadSound(const std::string& path);

        /**
         * @brief Load sound from file.
         * @param path Path to the sound file
         * @param alias Alias that will be used to access the sound
         * @return Unique ID of loaded sound
         */
        static size_t LoadSound(const std::string& path, const std::string& alias);

        /**
         * @brief Retrieve the default sound.
         * @return Reference to the default `sf::SoundBuffer`.
         */
        static sf::SoundBuffer& GetDefaultSound();

        /**
         * @brief Retrieve a sound buffer by its ID.
         * @param soundId The unique ID of the sound buffer to retrieve.
         * @return Reference to the requested `sf::SoundBuffer`.
         */
        static sf::SoundBuffer& GetSound(size_t soundId);

        /**
         * @brief Retrieve a sound buffer by its alias.
         * @param soundAlias The alias of the sound buffer to retrieve.
         * @return Reference to the requested `sf::SoundBuffer`.
         */
        static sf::SoundBuffer& GetSound(const std::string& soundAlias);

        /**
         * @brief Unload all loaded assets, including textures, sounds, fonts, and tile maps and others.
         *
         * This method clears all internal storage and resets the asset manager to its initial state.
         */
        static void UnloadAll();

    protected:
        Assets();

        Assets(const Assets&) = delete;

        Assets& operator=(const Assets&) = delete;

        /**
         * @brief Retrieve static instance of Asset Manager.
         * @return Reference to the static instance.
         */
        static Assets* GetInstance() {
            static Assets instance;
            return &instance;
        }

        static void LoadTerrainLayerData(const Terrain::LayerDefinition* terrainLayerDefinition, Terrain::TileMap& map);
        static void LoadFeatureLayerData(const Terrain::LayerDefinition* featuresLayerDefinition, Terrain::TileMap& map);

        static void ReadNavDataForLayer(Terrain::TileMap& map, Terrain::Layer& layer, const Terrain::LayerDefinition* layerDefinition);

        std::vector<Terrain::TileMap> _maps;
        std::unordered_map<std::string, size_t> _mapAliases;

        std::vector<sf::Texture> _textures;
        std::unordered_map<std::string, size_t> _textureAliases;
        std::unordered_map<size_t, Animation::SpriteSheet> _animationSheets;

        std::vector<sf::Font> _fonts;
        std::unordered_map<std::string, sf::Font> _fontAliases;

        std::vector<sf::SoundBuffer> _sounds;
        std::unordered_map<std::string, size_t> _soundAliases;
    };
}
