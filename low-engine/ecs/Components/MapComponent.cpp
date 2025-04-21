#include "MapComponent.h"

namespace LowEngine::ECS {
    void MapComponent::Update(float deltaTime) {
        auto& map = Assets::GetMap(m_MapId);
        map.Update(deltaTime);

        auto transformComponent = _memory->GetComponent<TransformComponent>(EntityId);
        m_Sprite.setPosition(transformComponent->Position);
        m_Sprite.setRotation(transformComponent->Rotation);
        m_Sprite.setScale(transformComponent->Scale);
        m_Sprite.Layer = Layer;
    }

    LowEngine::Sprite* MapComponent::Draw() {
        auto& map = Assets::GetMap(m_MapId);

        m_Texture.clear();
        m_Texture.draw(*map.TerrainLayer.GetDrawable());
        m_Texture.draw(*map.FeaturesLayer.GetDrawable());
        m_Texture.display();

        m_Sprite.setTexture(m_Texture.getTexture());
        return &m_Sprite;
    }

    void MapComponent::SetMapId(size_t mapId) {
        auto& map = Assets::GetMap(mapId);

        m_MapId = mapId;

        if (!m_Texture.resize({map.Width, map.Height})) {
            _log->error("Failed to resize map render texture to {}x{}.", map.Width, map.Height);
        }
        m_Texture.clear();

        m_Sprite.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(map.Width), static_cast<int>(map.Height)}));
    }
}
