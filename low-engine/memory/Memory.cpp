#include "Memory.h"

LowEngine::Memory::Memory::Memory(Scene& scene) : _entities() {
    for (size_t i = 0; i < _entities.size(); i++) {
        new (&_entities[i]) ECS::Entity(scene);
    }
    _entities[0].InitAsDefault();

    // initialize default transform component
    _transformComponents[0].InitAsDefault();
}

LowEngine::ECS::Entity& LowEngine::Memory::Memory::CreateEntity(const std::string& name) {
    for (uint32_t i = 0; i < _entities.size(); i++) {
        if (!_entities[i].Active) {
            _entities[i].Activate(name);
            return _entities[i];
        }
    }
    return _entities[0]; // return default entity
}

// uint32_t LowEngine::Memory::Memory::CreateEntity(const std::string& name) {
//     for (uint32_t i=0; i<_entities.size(); i++) {
//         if (!_entities[i].Active) {
//             _entities[i].Activate(name);
//             return i;
//         }
//     }
//     return 0; // return Id of default entity
// }

template<typename T, typename... Args>
T& LowEngine::Memory::Memory::CreateComponent(uint32_t entityId, Args&&... args) {
    static_assert(std::is_base_of_v<ECS::Component, T>, "T must be derived from Component");

    if constexpr (std::is_same_v<T, ECS::TransformComponent>) {
        for (uint32_t i = 0; i < _transformComponents.size(); i++) {
            if (!_transformComponents[i].Active) {
                _entities[entityId].Transform = _transformComponents[i];
                _transformComponents[i].Activate(entityId, std::forward<Args>(args)...);
                return _transformComponents[i];
            }
        }
    } else {
        static_assert(!sizeof(T*), "No memory arena defined for this Component type.");
    }

    // assign default transform component to entity
    _entities[entityId].TransformID = 0;
    return _transformComponents[0];
}

// template<typename T, typename ... Args>
// uint32_t LowEngine::Memory::Memory::CreateComponent(uint32_t entityId, Args&&... args) {
//     static_assert(std::is_base_of_v<ECS::Component, T>, "T must be derived from Component");
//
//     if constexpr (std::is_same_v<T, ECS::TransformComponent>) {
//         for (uint32_t i=0; i<_transformComponents.size(); i++) {
//             if (!_transformComponents[i].Active) {
//                 _entities[entityId].TransformID = i;
//                 _transformComponents[i].Activate(entityId, std::forward<Args>(args)...);
//                 return i;
//             }
//         }
//     }
//     else {
//         static_assert(!sizeof(T*), "No memory arena defined for this Component type.");
//     }
//
//     // assign default transform component to entity
//     _entities[entityId].TransformID = 0;
//     return 0;
// }

template uint32_t LowEngine::Memory::Memory::CreateComponent<LowEngine::ECS::TransformComponent>(uint32_t entityId);
