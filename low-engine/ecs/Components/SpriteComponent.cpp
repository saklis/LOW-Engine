#include "SpriteComponent.h"

namespace LowEngine::ECS {
    SpriteComponent::SpriteComponent() : Sprite(LowEngine::Assets::GetDefaultTexture()) {
    }

    void SpriteComponent::SetSprite(const sf::Texture& texture) {
        Sprite.setTexture(texture);
        Sprite.setTextureRect(sf::IntRect({0, 0}, static_cast<sf::Vector2<int>>(texture.getSize())));
    }

    void SpriteComponent::SetSprite(const std::string& textureAlias) {
        SetSprite(LowEngine::Assets::GetTexture(textureAlias));
    }

    void SpriteComponent::SetSprite(int32_t textureId) {
        SetSprite(LowEngine::Assets::GetTexture(textureId));
    }

    void SpriteComponent::ApplyTransforms(const LowEngine::ECS::TransformComponent& transforms) {
        Sprite.setPosition(transforms.Position);
        Sprite.setRotation(transforms.Rotation);
        Sprite.setScale(transforms.Scale);
    }
}
