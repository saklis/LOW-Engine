
#include "Scene.h"
namespace LowEngine {
    Scene::Scene(const std::string& name): Name(name), _memory() {
    }

    void Scene::InitAsDefault() {
        Active = true;
        Name = "Default scene";
    }

    void Scene::Update() {
        for (auto& entity : _memory.GetEntities()) {
            if (entity.Active) {
                auto& spriteComponent = _memory.GetComponent<ECS::SpriteComponent>(entity.Id);
                auto& transformComponent = _memory.GetComponent<ECS::TransformComponent>(entity.Id);

                spriteComponent.ApplyTransforms(transformComponent);
            }
        }
    }

    void Scene::Draw(sf::RenderWindow& window) {
        auto spritesView = _memory.GetAllComponents<ECS::SpriteComponent>();
        for (auto& sprite : spritesView) {
            window.draw(sprite.Sprite);
        }
    }

    uint32_t Scene::AddEntity(const std::string& name) {
        return _memory.CreateEntity(name);
    }

    void Scene::Destroy() {
        _memory.Destroy();
    }
}
