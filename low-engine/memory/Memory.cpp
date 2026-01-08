#include "Memory.h"

namespace LowEngine::Memory {
    Memory::Memory() {
        // do nothing
    }

    Memory::Memory(Memory const& other) : _typeInfos(other._typeInfos) {
        _nextTypeId = other._nextTypeId;

        // clone entities
        for (auto const& entPtr: other._entities) {
            if (entPtr == nullptr) continue;

            _entities.emplace_back(
                std::unique_ptr<ECS::IEntity>(entPtr->Clone(this))
            );
            _entities.back()->Id = _entities.size() - 1;
        }

        // clone components
        for (auto const& [typeIdx, poolPtr]: other._components) {
            _components[typeIdx] = poolPtr->Clone(this);
        }
    }

    void Memory::UpdateAllComponents(float deltaTime) {
        for (auto& component: _components) {
            component.second->Update(deltaTime);
        }
    }

    nlohmann::ordered_json Memory::SerializeAllEntitiesToJSON() {
		nlohmann::ordered_json entitiesJson = nlohmann::ordered_json::array();
        for (const auto& entity : _entities) {
            if (entity != nullptr) {
                entitiesJson.push_back(entity->SerializeToJSON());
            }
        }

		return entitiesJson;
    }

    nlohmann::ordered_json Memory::SerializeAllComponentsToJSON() {
		nlohmann::ordered_json componentsJson = nlohmann::ordered_json::array();
        for (const auto& [type, pool] : _components) {
            nlohmann::ordered_json poolJson = pool->SerializeToJSON();
            componentsJson.push_back(poolJson);
		}

		return componentsJson;
    }

    bool Memory::DeserializeAllComponentsFromJSON(const nlohmann::ordered_json& jsonData) {
        for (const auto& componentsJson : jsonData) {
            for (int i = 0; i < componentsJson.size(); ++i) {
                auto& componentJson = componentsJson[i];
                size_t entityId = componentJson["EntityId"];
                std::string typeName = componentJson["Type"];
                for (auto& typeInfoPair : _typeInfos) {
                    const TypeInfo& typeInfo = typeInfoPair.second;
                    if (typeInfo.TypeName == typeName) {
                        if (!typeInfo.DeserializeFromJSON(entityId, componentJson)) {
                            _log->error("Failed to deserialize component of type '{}' for entity with id '{}'",
                                typeName, entityId);
                            return false;
                        }
                    }
                }
            }
        }

        return true;
    }

    void Memory::CollectSprites(std::vector<Sprite>& sprites) {
        for (auto& [type, pool]: _components) {
            pool->CollectSprites(sprites);
        }
    }

    void Memory::Destroy() {
        _entities.clear();
        for (auto& component: _components) {
            component.second.reset();
        }
        _components.clear();
    }
}
