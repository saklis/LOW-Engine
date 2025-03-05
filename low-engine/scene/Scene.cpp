
#include "Scene.h"

LowEngine::Scene::Scene(const std::string& name): Name(name), _memory() {
}

void LowEngine::Scene::InitAsDefault() {
    Active = true;
    Name = "Default scene";
}

void LowEngine::Scene::Update() {
    for (auto& entity : _memory.GetEntities()) {
        if (entity.Active) {
            auto& spriteComponent = _memory.GetComponent<ECS::SpriteComponent>(entity.Id);
            auto& transformComponent = _memory.GetComponent<ECS::TransformComponent>(entity.Id);

            spriteComponent.ApplyTransforms(transformComponent);
        }
    }
}

void LowEngine::Scene::Draw(sf::RenderWindow& window) {
    auto spritesView = _memory.GetAllComponents<ECS::SpriteComponent>();
    for (auto& sprite : spritesView) {
        window.draw(sprite.Sprite);
    }
}

uint32_t LowEngine::Scene::AddEntity(const std::string& name) {
    return _memory.CreateEntity(name);
}

void LowEngine::Scene::Destroy() {
    _memory.Destroy();
}
