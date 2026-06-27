#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "Tile.h"
#include "assets/Assets.h"
#include "graphics/Sprite.h"
#include "graphics/Drawables.h"
#include "utils/TypeHash.h"
#include "utils/Uuid.h"

namespace LowEngine::TileMap {
    class TileMapLayer {
    public:
        TileMapLayer() = default;

        TileMapLayer(const TileMapLayer& other);
        TileMapLayer& operator=(const TileMapLayer& other);

        TileMapLayer(TileMapLayer&& other) noexcept;
        TileMapLayer& operator=(TileMapLayer&& other) noexcept;

        std::string Id = Utils::GetNewUuidV4();

        /**
         * @brief Name of this layer.
         */
        std::string Name = "Terrain Layer";

        bool IsVisible = true;

        /**
         * @brief Whether this layer participates in navigation grid baking.
         *
         * When true, tiles on this layer contribute their TraversalMask and
         * EntryCost to the baked NavigationGrid. Decoration layers (overlays,
         * fog, effects) should leave this false so they are ignored by the
         * pathfinder.
         */
        bool ContributesToNavigation = false;

        bool ContributesToCollision = false;

        /**
         * @brief Size of a tile on this layer.
         */
        sf::Vector2<std::size_t> TileSize = sf::Vector2<std::size_t>();

        void SetTextureId(std::size_t textureId);

        std::size_t GetTextureId();

        int GetDrawOrder();

        void SetDrawOrder(int drawOrder);

        void AddTile(sf::Vector2i cellCoords, sf::IntRect spritesheetCoords, bool skipRebuild = false);

        void AddTile(sf::Vector2i cellCoords, std::string& animClipName);

        /**
         * @brief Look up the tile at the given cell coordinates.
         *
         * @param cellCoords Grid cell coordinates (col, row) to query.
         * @return Pointer to the tile, or nullptr if no tile is present at that cell.
         */
        [[nodiscard]] const Tile* FindTile(sf::Vector2i cellCoords) const;

        [[nodiscard]] Tile* FindTile(sf::Vector2i cellCoords);

        bool DeleteTile(sf::Vector2i cellCoords);

        [[nodiscard]] const std::unordered_map<sf::Vector2i, Tile, Utils::Vector2iHash>& GetTiles() const {
            return _tiles;
        }

        void CollectDrawables(std::vector<SceneDrawable>& drawables);

        void Update(float deltaTime, Animation::SpriteSheet& spriteSheet);

        nlohmann::ordered_json SerializeToJSON();

        bool DeserializeFromJSON(const nlohmann::ordered_json& json);

    protected:
        /**
         * @brief The draw order of all sprites in this layer.
         */
        int _drawOrder = 0;

        /**
         * @brief Id of the texture (spritesheet) used by this layer.
         */
        std::size_t _textureId = 0;

        /**
         * @brief Tiles on this layer.
         *
         * Key: grid cell coordinates (col, row)
         * Value: tile data
         */
        std::unordered_map<sf::Vector2i, Tile, Utils::Vector2iHash> _tiles;

        /**
         * @brief Vertex array for static tiles. Built once on tile placement, never updated per-frame.
         */
        sf::VertexArray _staticVertices{sf::PrimitiveType::Triangles};
        std::unordered_map<sf::Vector2i, std::size_t, Utils::Vector2iHash> _staticVertexIndex;

        /**
         * @brief Vertex array for animated tiles. UVs updated each frame as animation advances.
         */
        sf::VertexArray _animVertices{sf::PrimitiveType::Triangles};
        std::unordered_map<sf::Vector2i, std::size_t, Utils::Vector2iHash> _animVertexIndex;

        void RebuildStaticVertices();
        void RebuildAnimVertices();
        void UpdateAnimVertexUVs(std::size_t idx, const sf::IntRect& rect);
    };
}
