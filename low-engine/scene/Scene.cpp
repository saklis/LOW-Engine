#include "Scene.h"

namespace LowEngine {
    Scene::Scene(const std::string& name): Name(name), _memory() {
    }

    void Scene::InitAsDefault() {
        Active = true;
        Name = "Default scene";
    }

    void Scene::Update(float deltaTime) {
        _memory.Update(deltaTime);

        for (auto& entity: _memory.GetEntities()) {
            if (entity.Active) {
                auto transformComponent = _memory.GetComponent<ECS::TransformComponent>(entity.Id);

                auto spriteComponent = _memory.GetComponent<ECS::SpriteComponent>(entity.Id);
                if (spriteComponent) spriteComponent->ApplyTransforms(transformComponent);

                auto animatedSpriteComponent = _memory.GetComponent<ECS::AnimatedSpriteComponent>(entity.Id);
                if (animatedSpriteComponent) {
                    animatedSpriteComponent->ApplyTransforms(transformComponent);
                    animatedSpriteComponent->Update(deltaTime);
                }
            }
        }
    }

    void Scene::Draw(sf::RenderWindow& window) {
        auto spritesView = _memory.GetAllComponents<ECS::SpriteComponent>();
        for (auto& sprite: spritesView) {
            window.draw(sprite.Sprite);
        }
        auto animatedSpritesView = _memory.GetAllComponents<ECS::AnimatedSpriteComponent>();
        for (auto& animatedSprite: animatedSpritesView) {
            window.draw(animatedSprite.Sprite);
        }
    }

    unsigned int Scene::AddEntity(const std::string& name) {
        return _memory.CreateEntity(name);
    }

    std::vector<ECS::Entity>& Scene::GetEntities() {
        return _memory.GetEntities();
    }

    void* Scene::GetComponent(unsigned int entity_id, std::type_index typeIndex) {
        return _memory.GetComponent(entity_id, typeIndex);
    }

    void Scene::Destroy() {
        _memory.Destroy();
    }
}
