#include <algorithm>

#include "Scene.h"
#include "ecs/ECSHeaders.h"

namespace LowEngine {
    Scene::Scene(const std::string& name): Name(name), _memory() {
    }

    void Scene::InitAsDefault() {
        Active = true;
        Name = "Default scene";
    }

    void Scene::Update(float deltaTime) {
        _memory.UpdateAllComponents(deltaTime);
    }

    void Scene::Draw(sf::RenderWindow& window) {
        auto cameraComponent = _memory.GetComponent<ECS::CameraComponent>(_cameraEntityId);
        if (cameraComponent) {
            cameraComponent->SetView(window);
        }

        std::vector<sf::Sprite> sprites;

        auto spritesView = _memory.GetAllComponents<ECS::SpriteComponent>();
        for (auto& spriteData: *spritesView) {
            sprites.emplace_back(reinterpret_cast<ECS::SpriteComponent*>(&spriteData)->Sprite);
        }

        auto animatedSpritesView = _memory.GetAllComponents<ECS::AnimatedSpriteComponent>();
        for (auto& animatedSpriteData: *animatedSpritesView) {
            sprites.emplace_back(reinterpret_cast<ECS::AnimatedSpriteComponent*>(&animatedSpriteData)->Sprite);
        }

        std::sort(sprites.begin(), sprites.end(), [](const sf::Sprite& a, const sf::Sprite& b) {
            return a.getPosition().y < b.getPosition().y;
        });

        for (auto& sprite: sprites) {
            window.draw(sprite);
        }
    }

    ECS::Entity* Scene::AddEntity(const std::string& name) {
        return _memory.CreateEntity<ECS::Entity>(name);
    }

    ECS::Entity* Scene::GetEntity(unsigned int entityId) {
        return _memory.GetEntity<ECS::Entity>(entityId);
    }

    std::vector<std::unique_ptr<ECS::IEntity>>* Scene::GetEntities() {
        return _memory.GetAllEntities();
    }

    void* Scene::GetComponent(unsigned int entity_id, std::type_index typeIndex) {
        return _memory.GetComponent(entity_id, typeIndex);
    }

    bool Scene::SetCurrentCamera(int entityId) {
        auto cameraComp = _memory.GetComponent<ECS::CameraComponent>(entityId);
        if (cameraComp == nullptr) {
            _log->warn("Entity {} does not have a camera component", entityId);
            return false;
        }

        _cameraEntityId = entityId;
        return true;
    }

    void Scene::SetWindowSize(sf::Vector2f windowSize) {
        if (_cameraEntityId == -1) {
            _log->warn("No camera entity set");
            return;
        }

        auto camera = _memory.GetComponent<ECS::CameraComponent>(_cameraEntityId);
        if (camera != nullptr) {
            camera->SetWindowSize(windowSize);
        } else {
            _log->warn("Current camera entity {} does not have a camera component", _cameraEntityId);
        }
    }

    void Scene::Destroy() {
        _memory.Destroy();
    }
}
