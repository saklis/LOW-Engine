#include "TileMapLayer.h"

namespace LowEngine::TileMap {
    static void rebindTexture(Sprite& sprite, const sf::Texture& texture) {
        if (texture.getSize().x > 0)
            sprite.setTexture(texture);
    }

    TileMapLayer::TileMapLayer(const TileMapLayer& other)
        : Name(other.Name), _drawOrder(other._drawOrder), TileSize(other.TileSize),
          _textureId(other._textureId), _tiles(other._tiles),
          _textureSizeOffset(other._textureSizeOffset),
          _sourceImage(other._sourceImage), _image(other._image),
          _texture(other._texture), _sprite(other._sprite) {
        rebindTexture(_sprite, _texture);
    }

    TileMapLayer& TileMapLayer::operator=(const TileMapLayer& other) {
        if (this != &other) {
            Name = other.Name;
            _drawOrder = other._drawOrder;
            TileSize = other.TileSize;
            _textureId = other._textureId;
            _tiles = other._tiles;
            _textureSizeOffset = other._textureSizeOffset;
            _sourceImage = other._sourceImage;
            _image = other._image;
            _texture = other._texture;
            _sprite = other._sprite;
            rebindTexture(_sprite, _texture);
        }
        return *this;
    }

    TileMapLayer::TileMapLayer(TileMapLayer&& other) noexcept
        : Name(std::move(other.Name)), _drawOrder(other._drawOrder), TileSize(other.TileSize),
          _textureId(other._textureId), _tiles(std::move(other._tiles)),
          _textureSizeOffset(other._textureSizeOffset),
          _sourceImage(std::move(other._sourceImage)), _image(std::move(other._image)),
          _texture(std::move(other._texture)), _sprite(std::move(other._sprite)) {
        rebindTexture(_sprite, _texture);
    }

    TileMapLayer& TileMapLayer::operator=(TileMapLayer&& other) noexcept {
        if (this != &other) {
            Name = std::move(other.Name);
            _drawOrder = other._drawOrder;
            TileSize = other.TileSize;
            _textureId = other._textureId;
            _tiles = std::move(other._tiles);
            _textureSizeOffset = other._textureSizeOffset;
            _sourceImage = std::move(other._sourceImage);
            _image = std::move(other._image);
            _texture = std::move(other._texture);
            _sprite = std::move(other._sprite);
            rebindTexture(_sprite, _texture);
        }
        return *this;
    }

    void TileMapLayer::SetTextureId(std::size_t textureId) {
        _textureId = textureId;
        _sourceImage = Assets::GetTexture(_textureId).copyToImage();
    }

    std::size_t TileMapLayer::GetTextureId() {
        return _textureId;
    }

    int TileMapLayer::GetDrawOrder() {
        return _drawOrder;
    }

    void TileMapLayer::SetDrawOrder(int drawOrder) {
        _drawOrder = drawOrder;
        _sprite.DrawOrder = _drawOrder;
    }

    void TileMapLayer::AddTile(sf::Vector2i cellCoords, sf::IntRect spritesheetCoords, bool skipSpriteUpdate) {
        _tiles[cellCoords].Type = TileType::Static;
        _tiles[cellCoords].SpriteRect = spritesheetCoords;
        if (!skipSpriteUpdate) UpdateSprite();
    }

    void TileMapLayer::AddTile(sf::Vector2i cellCoords, std::string& animClipName) {
        _tiles[cellCoords].Type = TileType::Animated;
        _tiles[cellCoords].AnimationClipName = animClipName;
        // no UpdateSprite! Animated tiles are not baked into sprite
    }

    void TileMapLayer::CollectSprites(std::vector<Sprite>& sprites) {
        if (IsVisible && !_tiles.empty()) {
            // send backed in static tiles
            sprites.emplace_back(_sprite);

            // add animated tiles
            for (auto& [coords, tile]: _tiles) {
                if (tile.Type != TileType::Animated) continue;

                Sprite animSprite(Assets::GetTexture(_textureId));
                animSprite.setTextureRect(tile.SpriteRect);
                animSprite.setPosition({
                    static_cast<float>(coords.x * static_cast<int>(TileSize.x)),
                    static_cast<float>(coords.y * static_cast<int>(TileSize.y))
                });
                animSprite.DrawOrder = _drawOrder;
                sprites.emplace_back(animSprite);
            }
        }
    }

    void TileMapLayer::Update(float deltaTime, Animation::SpriteSheet& spriteSheet) {
        for (auto& [coords, tile]: _tiles) {
            if (tile.Type != TileType::Animated) continue;
            if (!spriteSheet.HasAnimationClip(tile.AnimationClipName)) continue;

            auto& clip = spriteSheet.GetAnimationClip(tile.AnimationClipName);
            tile.AnimSpriteTimer += deltaTime;
            if (tile.AnimSpriteTimer >= clip.FrameDuration) {
                tile.AnimSpriteTimer -= clip.FrameDuration;
                tile.AnimSpriteCurrentFrame = (tile.AnimSpriteCurrentFrame + 1) % clip.FrameCount;
                tile.SpriteRect = clip.Frames[tile.AnimSpriteCurrentFrame];
            }
        }
    }

    bool TileMapLayer::DeserializeFromJSON(const nlohmann::ordered_json& json) {
        if (json.contains("name")) {
            Name = json["name"].get<std::string>();
        }
        if (json.contains("isVisible")) {
            IsVisible = json["isVisible"].get<bool>();
        }
        if (json.contains("tileSize")) {
            TileSize.x = json["tileSize"]["x"].get<std::size_t>();
            TileSize.y = json["tileSize"]["y"].get<std::size_t>();
        }
        if (json.contains("textureAlias")) {
            SetTextureId(Assets::GetTextureId(json["textureAlias"].get<std::string>()));
        } else if (json.contains("textureId")) {
            // legacy: fallback to raw numeric ID (may assign wrong texture if load order changed)
            SetTextureId(json["textureId"].get<std::size_t>());
        }
        if (json.contains("drawOrder")) {
            SetDrawOrder(json["drawOrder"].get<int>());
        }
        if (json.contains("tiles")) {
            for (auto& tileJson : json["tiles"]) {
                sf::Vector2i coords = {tileJson["cellX"].get<int>(), tileJson["cellY"].get<int>()};
                auto type = static_cast<TileType>(tileJson["type"].get<std::uint8_t>());
                if (type == TileType::Animated) {
                    auto clipName = tileJson["animClipName"].get<std::string>();
                    AddTile(coords, clipName);
                } else {
                    sf::IntRect rect(
                        {tileJson["spriteRect"]["x"].get<int>(), tileJson["spriteRect"]["y"].get<int>()},
                        {tileJson["spriteRect"]["w"].get<int>(), tileJson["spriteRect"]["h"].get<int>()}
                    );
                    AddTile(coords, rect, true);
                }
            }
        }
        UpdateSprite();
        return true;
    }

    nlohmann::ordered_json TileMapLayer::SerializeToJSON() {
        nlohmann::ordered_json json;
        json["name"] = Name;
        json["isVisible"] = IsVisible;
        json["tileSize"] = {{"x", TileSize.x}, {"y", TileSize.y}};
        json["drawOrder"] = _drawOrder;
        json["textureAlias"] = Assets::GetTextureAlias(_textureId);

        nlohmann::ordered_json tilesJson = nlohmann::ordered_json::array();
        for (auto& [coords, tile] : _tiles) {
            nlohmann::ordered_json tileJson;
            tileJson["cellX"] = coords.x;
            tileJson["cellY"] = coords.y;
            tileJson["type"] = static_cast<std::uint8_t>(tile.Type);
            tileJson["spriteRect"] = {
                {"x", tile.SpriteRect.position.x},
                {"y", tile.SpriteRect.position.y},
                {"w", tile.SpriteRect.size.x},
                {"h", tile.SpriteRect.size.y}
            };
            tileJson["animClipName"] = tile.AnimationClipName;
            tilesJson.emplace_back(tileJson);
        }
        json["tiles"] = tilesJson;

        return json;
    }

    void TileMapLayer::UpdateSprite() {
        _image.resize(
            {static_cast<unsigned>(_textureSizeOffset * 2), static_cast<unsigned>(_textureSizeOffset * 2)},
            sf::Color::Transparent);

        if (Assets::HasSpriteSheet(_textureId)) {
            for (auto& [coords, tile]: _tiles) {
                // bake in only static tiles
                if (tile.Type != TileType::Static) continue;

                sf::Vector2i gridPos = coords;
                sf::Vector2f imagePos = {
                    gridPos.x * static_cast<float>(TileSize.x),
                    gridPos.y * static_cast<float>(TileSize.y)
                };

                for (size_t y = 0; y < tile.SpriteRect.size.y; y++) {
                    for (size_t x = 0; x < tile.SpriteRect.size.x; x++) {
                        auto color = _sourceImage.getPixel({
                            static_cast<unsigned>(tile.SpriteRect.position.x + x),
                            static_cast<unsigned>(tile.SpriteRect.position.y + y)
                        });
                        _image.setPixel({
                                            static_cast<unsigned>(imagePos.x + x + _textureSizeOffset),
                                            static_cast<unsigned>(imagePos.y + y + _textureSizeOffset)
                                        }, color);
                    }
                }
            }

            if (_texture.loadFromImage(_image)) {
                _sprite.setTexture(_texture);
            }
        }
    }
}
