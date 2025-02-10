#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <vector>

#include "ecs/Entity.h"
#include "ecs/Components/TransformComponent.h"
#include "ecs/Components/SpriteComponent.h"
#include "ecs/Components/ScriptComponent.h"

namespace LowEngine::Memory {
    class Memory {
    public:
        Memory();

        uint32_t CreateEntity(const std::string& name);

template<typename T, typename... Args>
uint32_t CreateComponent(uint32_t entityId, Args&&... args) {
    static_assert(std::is_base_of_v<ECS::Component, T>, "T must be derived from Component");

    if constexpr (std::is_base_of_v<T, ECS::TransformComponent>) {
        for (uint32_t i = 0; i < _transformComponents.size(); i++) {
            if (!_transformComponents[i].Active) {
                _entities[entityId].TransformId = i;
                _transformComponents[i].Activate(entityId, std::forward<Args>(args)...);
                return i;
            }
        }
        // assign default transform component
        _entities[entityId].TransformId = 0;
        return 0;
    } else if constexpr (std::is_base_of_v<T, ECS::SpriteComponent>) {
        for (uint32_t i = 0; i < _spriteComponents.size(); i++) {
            if (!_spriteComponents[i].Active) {
                _entities[entityId].SpriteId = i;
                _spriteComponents[i].Activate(entityId, std::forward<Args>(args)...);
                return i;
            }
        }
        // assign default sprite component
        _entities[entityId].SpriteId = 0;
        return 0;
    } else if constexpr (std::is_base_of_v<T, ECS::Component>) { // custom components defined in game code
        _customComponents.emplace_back(T());

        uint32_t componentId = _customComponents.size() - 1;
        _entities[entityId].CustomComponents.emplace_back(componentId);
        _customComponents.back().Activate(entityId, std::forward<Args>(args)...);
        return componentId;
    } else {
        static_assert(!sizeof(T*), "No memory arena defined for this Component type.");
    }
}

template<typename T>
T& GetComponent(uint32_t entityId) {
    if constexpr (std::is_base_of_v<T, ECS::TransformComponent>) {
        if (_entities[entityId].Active == true) {
            if (_entities[entityId].TransformId != 0) {
                if (_transformComponents[_entities[entityId].TransformId].Active == true) {
                    return _transformComponents[_entities[entityId].TransformId];
                }
            }
        }
        return _transformComponents[0];
    } else if constexpr (std::is_base_of_v<T, ECS::SpriteComponent>) {
        if (_entities[entityId].Active == true) {
            if (_entities[entityId].SpriteId != 0) {
                if (_spriteComponents[_entities[entityId].SpriteId].Active == true) {
                    return _spriteComponents[_entities[entityId].SpriteId];
                }
            }
        }
        return _spriteComponents[0];
    } else if constexpr (std::is_base_of_v<T, ECS::Component>) {
        for (auto& component : _customComponents) {
            if (std::is_same_v<T, decltype(component)>) {
                return component;
            }
        }
        return _customComponents[0];
    } else {
        static_assert(!sizeof(T*), "No memory arena defined for this Component type.");
    }
}

    protected:
        std::array<ECS::Entity, 1000> _entities;
        std::array<ECS::TransformComponent, 1000> _transformComponents;
        std::array<ECS::SpriteComponent, 1000> _spriteComponents;
        std::vector<ECS::Component> _customComponents;
    };
}
