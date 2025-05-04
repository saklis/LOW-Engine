#include "MapComponent.h"

namespace LowEngine::ECS {
    void MapComponent::Update(float deltaTime) {
        auto& map = Assets::GetTileMap(_mapId);
        map.Update(deltaTime);

        auto transformComponent = _memory->GetComponent<TransformComponent>(EntityId);
        _sprite.setPosition(transformComponent->Position);
        _sprite.setRotation(transformComponent->Rotation);
        _sprite.setScale(transformComponent->Scale);
        _sprite.Layer = Layer;
    }

    LowEngine::Sprite* MapComponent::Draw() {
        auto& map = Assets::GetTileMap(_mapId);

        _texture.clear();
        _texture.draw(*map.TerrainLayer.GetDrawable());
        _texture.draw(*map.FeaturesLayer.GetDrawable());
        _texture.display();

        _sprite.setTexture(_texture.getTexture());
        return &_sprite;
    }

    void MapComponent::SetMapId(size_t mapId) {
        auto& map = Assets::GetTileMap(mapId);

        _mapId = mapId;

        if (!_texture.resize({map.Width, map.Height})) {
            _log->error("Failed to resize map render texture to {}x{}.", map.Width, map.Height);
        }
        _texture.clear();

        _sprite.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(map.Width), static_cast<int>(map.Height)}));
    }
}
