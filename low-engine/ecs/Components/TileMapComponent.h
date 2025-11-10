#pragma once

#include "ecs/IComponent.h"
#include "TransformComponent.h"
#include "graphics/Sprite.h"

namespace LowEngine::ECS {
    /**
     * @brief Represents a component that allows adding Tile Map to an Entity
     *
     * Depends on Transform Component
     */
    class TileMapComponent : public IComponent<TileMapComponent, TransformComponent> {
    public:
        /**
         * @brief Layer number.
         *
         * Sprite of this component will be drawn on this layer.
         * This applies only if Scene's sorting mode is set to Layer.
         */
        int Layer = 0;

        explicit TileMapComponent(Memory::Memory* memory)
            : IComponent(memory), _sprite(Assets::GetDefaultTexture()) {
        }

        TileMapComponent(Memory::Memory* memory, TileMapComponent const* other)
            : IComponent(memory, other), _sprite(other->_sprite), _mapId(other->_mapId), Layer(other->Layer) {

            auto& map = Assets::GetTileMap(_mapId);
            Resize(map);
        }

        ~TileMapComponent() override = default;

        void Initialize() override {
        }

        void Update(float deltaTime) override;

        Sprite* Draw() override;

        /**
         * @brief Set Id of the Tile Map to be used.
         * @param mapId Id of a Tile Map
         */
        void SetMapId(size_t mapId);

        std::vector<sf::Vector2f> FindPath(sf::Vector2f start, sf::Vector2f end, Terrain::Navigation::MovementType movementType);

    protected:
        size_t _mapId = -1;

        /**
         * @brief A Sprite instance that is used to create drawable from all layers
         */
        Sprite _sprite;

        /**
         * @brief Internal texture used as a source for _sprite.
         */
        sf::RenderTexture _texture;

        /**
         * @brief Resize internal _texture and _sprite to match provided map asset.
         * @param map Reference to map asset to mach size to
         */
        void Resize(Terrain::TileMap& map);
    };
}
