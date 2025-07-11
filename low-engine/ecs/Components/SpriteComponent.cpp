#include "SpriteComponent.h"

namespace LowEngine::ECS {
    void SpriteComponent::SetTexture(const sf::Texture& texture) {
        Sprite.setTexture(texture);

        auto size = static_cast<sf::Vector2<int>>(texture.getSize());
        Sprite.setTextureRect(sf::IntRect({0, 0}, size));
        Sprite.setOrigin({static_cast<float>(size.x) / 2, static_cast<float>(size.y) / 2});
    }

    void SpriteComponent::Update(float deltaTime) {
        auto transformComponent = _memory->GetComponent<TransformComponent>(EntityId);
        Sprite.setPosition(transformComponent->Position);
        Sprite.setRotation(transformComponent->Rotation);
        Sprite.setScale(transformComponent->Scale);
        Sprite.Layer = Layer;
    }

    void SpriteComponent::SetTexture(const std::string& textureAlias) {
        size_t textureId = Assets::GetTextureId(textureAlias);
        SetTexture(textureId);
    }

    void SpriteComponent::SetTexture(size_t textureId) {
        TextureId = textureId;
        SetTexture(Assets::GetTexture(textureId));
    }
}
