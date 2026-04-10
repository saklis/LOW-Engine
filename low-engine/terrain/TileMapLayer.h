#pragma once
#include "Tile.h"
#include "assets/Assets.h"
#include "graphics/Sprite.h"
#include "utils/TypeHash.h"

namespace LowEngine::TileMap {
    class TileMapLayer {
    public:
        TileMapLayer() : _sprite(_texture) {
            _image.resize(
                {static_cast<unsigned>(_textureSizeOffset * 2), static_cast<unsigned>(_textureSizeOffset * 2)},
                sf::Color::Transparent);
            if (_texture.loadFromImage(_image)) {
                _sprite.setTextureRect(sf::IntRect({0, 0}, {
                                                       static_cast<int>(_textureSizeOffset * 2),
                                                       static_cast<int>(_textureSizeOffset * 2)
                                                   }));
                _sprite.setPosition({-_textureSizeOffset, -_textureSizeOffset});
            } else {
                _log->error("Error while creating texture for new layer!");
            }
        }

        TileMapLayer(const TileMapLayer& other);

        TileMapLayer& operator=(const TileMapLayer& other);

        TileMapLayer(TileMapLayer&& other) noexcept;

        TileMapLayer& operator=(TileMapLayer&& other) noexcept;

        /**
         * @brief Name of this layer.
         */
        std::string Name = "Terrain Layer";

        bool IsVisible = true;

        /**
         * @brief Size of a tile on this layer.
         */
        sf::Vector2<std::size_t> TileSize = sf::Vector2<std::size_t>();

        void SetTextureId(std::size_t textureId);

        std::size_t GetTextureId();

        int GetDrawOrder();

        void SetDrawOrder(int drawOrder);

        void AddTile(sf::Vector2i cellCoords, sf::IntRect spritesheetCoords, bool skipSpriteUpdate = false);

        void AddTile(sf::Vector2i cellCoords, std::string& animClipName);

        void CollectSprites(std::vector<Sprite>& sprites);

        void Update(float deltaTime, Animation::SpriteSheet& spriteSheet);

        nlohmann::ordered_json SerializeToJSON();

        bool DeserializeFromJSON(const nlohmann::ordered_json& json);

    protected:
        /**
         * @brief The draw order of all sprites in this layer.
         *
         * Sprites with lower draw orders will be drawn first, and thus appear behind sprites with higher draw orders.
         * The default draw order is 0.
         */
        int _drawOrder = 0;

        /**
         * @brief Id of the texture that is used by this layer as a source of tiles.
         */
        std::size_t _textureId = 0;

        /**
         * @brief Tile on this layer.
         *
         * Key: grid cell coordinates (col, row) (not pixels)
         * Value: spreadsheet coords.
         */
        std::unordered_map<sf::Vector2i, Tile, Utils::Vector2iHash> _tiles;

        float _textureSizeOffset = 4082;

        sf::Image _sourceImage;
        sf::Image _image;
        sf::Texture _texture;
        Sprite _sprite;

        void UpdateSprite();
    };
}
