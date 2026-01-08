#include <algorithm>

#include "Scene.h"

namespace LowEngine {
	Scene::Scene(): Name(""), _memory() {
        RegisterDefaultComponentTypes();

        auto worldDef = GetB2WorldDef();
        _box2dWorldId = b2CreateWorld(&worldDef);

		_memory.Box2dWorldId = _box2dWorldId;
	}

    Scene::Scene(const std::string& name): Name(name), _memory() {
        RegisterDefaultComponentTypes();

        auto worldDef = GetB2WorldDef();
        _box2dWorldId = b2CreateWorld(&worldDef);

        _memory.Box2dWorldId = _box2dWorldId;
    }

    Scene::Scene(Scene const& other, const std::string& nameSufix): Initialized(false) // don’t auto-activate the clone
                                      , IsPaused(true)
                                      , Name(other.Name + nameSufix)
                                      , _cameraEntityId(other._cameraEntityId)
                                      , _spriteSortingMethod(other._spriteSortingMethod)
                                      , _memory(other._memory) // calls Memory(const Memory&) → deep copy!
    {
        RegisterDefaultComponentTypes();

        auto worldDef = GetB2WorldDef();
        _box2dWorldId = b2CreateWorld(&worldDef);

        _memory.Box2dWorldId = _box2dWorldId;
    }

	void Scene::InitAsDefault() {
        Initialized = true;
        Name = "Default scene";
    }

    nlohmann::ordered_json Scene::SerializeToJSON() {
        nlohmann::ordered_json sceneJson;

		sceneJson["name"] = Name;
		sceneJson["entities"] = _memory.SerializeAllEntitiesToJSON();
        sceneJson["components"] = _memory.SerializeAllComponentsToJSON();

        return sceneJson;
	}

    bool Scene::DeserializeFromJSON(const nlohmann::ordered_json& jsonData) {
        if (jsonData.contains("name")) {
            Name = jsonData["name"].get<std::string>();
        } else {
			_log->error("Provided json data don't contain 'name' field for scene deserialization.");
			return false;
        }
        if (jsonData.contains("entities")) {
            if (!_memory.DeserializeAllEntitiesFromJSON<ECS::Entity>(jsonData["entities"]))
            {
				_log->error("Failed to deserialize entities for scene '{}'", Name);
				return false;
            }
        }
        if (jsonData.contains("components")) {
			if (!_memory.DeserializeAllComponentsFromJSON(jsonData["components"]))
            {
                _log->error("Failed to deserialize components for scene '{}'", Name);
                return false;
			}
		}

		return true;
	}

	void Scene::Update(float deltaTime) {
        if (IsPaused) return;

        _memory.UpdateAllComponents(deltaTime);
    }

	void Scene::FixedUpdate(float fixedDeltaTime) {
        if (IsPaused) return;

        b2World_Step(_box2dWorldId, fixedDeltaTime, 4);
        auto contactEvents = b2World_GetContactEvents(_box2dWorldId);
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
                std::ranges::sort(sprites, [](const Sprite& a, const Sprite& b) {
                    return a.getPosition().y < b.getPosition().y;
                });
                break;
            case SpriteSortingMethod::Layers:
                std::ranges::sort(sprites, [](const Sprite& a, const Sprite& b) {
                    return a.Layer < b.Layer;
                });
                break;
            case SpriteSortingMethod::None:
            default: /* no sorting */;
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

    bool Scene::IsEntitySafeToDestroy(size_t entityId) const {
        if (this->_cameraEntityId == entityId) return false;
        return true;
    }

    void Scene::DestroyEntity(size_t entityId) {
        auto entityToDelete = _memory.GetEntity<ECS::Entity>(entityId);
        if (entityToDelete) {
            _memory.DestroyEntity(entityToDelete);
        }else {
            _log->warn("Entity with Id '{}' does not exists, so it can't be deleted.", entityId);
        }
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

    void* Scene::GetComponent(size_t entityId, std::type_index typeIndex) {
        return _memory.GetComponent(entityId, typeIndex);
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

    ECS::Entity* Scene::GetCurrentCamera() {
        if (_cameraEntityId == Config::MAX_SIZE) {
            _log->debug("No camera entity set for scene '{}'", Name);
            return nullptr;
        }

        return _memory.GetEntity<ECS::Entity>(_cameraEntityId);
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
        _log->info("Destroying scene '{}'", Name);
		b2DestroyWorld(_box2dWorldId);
		_box2dWorldId = b2_nullWorldId;
        _memory.Destroy();
    }

	void Scene::RegisterDefaultComponentTypes() {
        _memory.RegisterComponentType<ECS::AnimatedSpriteComponent>();
		_memory.RegisterComponentType<ECS::CameraComponent>();
		_memory.RegisterComponentType<ECS::SoundComponent>();
		_memory.RegisterComponentType<ECS::SpriteComponent>();
		_memory.RegisterComponentType<ECS::TileMapComponent>();
		_memory.RegisterComponentType<ECS::TransformComponent>();
	}

	b2WorldDef Scene::GetB2WorldDef() {
        auto worldDef = b2DefaultWorldDef();
        
		worldDef.enableSleep = false;

        return worldDef;
	}
}
