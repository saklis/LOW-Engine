#include "SpriteComponent.h"

void LowEngine::ECS::SpriteComponent::SetSprite(const sf::Texture& texture) {
    _sprite.setTexture(texture);
}
