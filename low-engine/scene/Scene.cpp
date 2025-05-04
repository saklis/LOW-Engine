#include <algorithm>

#include "Scene.h"

namespace LowEngine {
    Scene::Scene(const std::string& name): Name(name), _memory() {
    }

    Scene::Scene(Scene const& other): Active(false) // don’t auto-activate the clone
                                      , IsPaused(true)
                                      , Name(other.Name + " (TEMPORARY)")
                                      , _cameraEntityId(other._cameraEntityId)
                                      , _spriteSortingMethod(other._spriteSortingMethod)
                                      , _memory(other._memory) // calls Memory(const Memory&) → deep copy!
    {
    }

    void Scene::InitAsDefault() {
        Active = true;
        Name = "Default scene";
    }

    void Scene::Update(float deltaTime) {
        _memory.UpdateAllComponents(deltaTime);
    }

    void Scene::Draw(sf::RenderWindow& window) {
        if (_cameraEntityId < Config::MAX_SIZE) {
            auto cameraComponent = _memory.GetComponent<ECS::CameraComponent>(_cameraEntityId);
            if (cameraComponent) {
                cameraComponent->SetView(window);
            }
        }

        std::vector<Sprite> sprites;
        _memory.CollectSprites(sprites);

        switch (_spriteSortingMethod) {
            case SpriteSortingMethod::YAxisIncremental:
                std::sort(sprites.begin(), sprites.end(), [](const Sprite& a, const Sprite& b) {
                    return a.getPosition().y < b.getPosition().y;
                });
                break;
            case SpriteSortingMethod::Layers:
                std::sort(sprites.begin(), sprites.end(), [](const Sprite& a, const Sprite& b) {
                    return a.Layer < b.Layer;
                });
                break;
            case SpriteSortingMethod::None:
            default: ;
        }

        for (auto& sprite: sprites) {
            window.draw(sprite);
        }
    }

    ECS::Entity* Scene::AddEntity(const std::string& name) {
        auto* entity = _memory.CreateEntity<ECS::Entity>(name);
        if (entity == nullptr) {
            _log->error("Failed to create entity '{}'", name);
            return nullptr;
        }

        _log->debug("Entity '{}' created with id {}", name, entity->Id);
        return entity;
    }


    ECS::Entity* Scene::GetEntity(unsigned int entityId) {
        return _memory.GetEntity<ECS::Entity>(entityId);
    }

    ECS::Entity* Scene::FindEntity(const std::string& name) {
        return _memory.FindEntity<ECS::Entity>(name);
    }

    std::vector<std::unique_ptr<ECS::IEntity> >* Scene::GetEntities() {
        return _memory.GetAllEntities();
    }

    void* Scene::GetComponent(unsigned int entity_id, std::type_index typeIndex) {
        return _memory.GetComponent(entity_id, typeIndex);
    }

    bool Scene::SetCurrentCamera(size_t entityId) {
        auto cameraComp = _memory.GetComponent<ECS::CameraComponent>(entityId);
        if (cameraComp == nullptr) {
            _log->warn("Entity {} does not have a camera component", entityId);
            return false;
        }

        _cameraEntityId = entityId;

        _log->debug("Camera entity set to {} for scene '{}'", entityId, Name);
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

    void Scene::SetSpriteSorting(SpriteSortingMethod method) {
        _spriteSortingMethod = method;

        _log->debug("Sprite sorting method for scene '{}' set to {}", Name, static_cast<int>(method));
    }

    void Scene::Destroy() {
        _memory.Destroy();
    }
}
