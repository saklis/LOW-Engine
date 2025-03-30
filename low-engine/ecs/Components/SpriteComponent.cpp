#include "SpriteComponent.h"

namespace LowEngine::ECS {
    SpriteComponent::SpriteComponent() : Sprite(LowEngine::Assets::GetDefaultTexture()) {
    }

    void SpriteComponent::SetSprite(const sf::Texture& texture) {
        Sprite.setTexture(texture);

        auto size = static_cast<sf::Vector2<int>>(texture.getSize());
        Sprite.setTextureRect(sf::IntRect({0, 0}, size));
        Sprite.setOrigin({static_cast<float>(size.x) / 2, static_cast<float>(size.y) / 2});
    }

    void SpriteComponent::Update(float deltaTime) {
        auto transformComponent = Memory->GetComponent<TransformComponent>(EntityId);
        Sprite.setPosition(transformComponent->Position);
        Sprite.setRotation(transformComponent->Rotation);
        Sprite.setScale(transformComponent->Scale);
    }

    void SpriteComponent::SetSprite(const std::string& textureAlias) {
        SetSprite(LowEngine::Assets::GetTexture(textureAlias));
    }

    void SpriteComponent::SetSprite(int textureId) {
        SetSprite(LowEngine::Assets::GetTexture(textureId));
    }
}
