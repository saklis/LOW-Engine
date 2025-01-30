#include "Memory.h"

Memory::Memory() {
    // initialize default entity
    _entities[0].ActivateAsDefault();

    // initialize default transform component
    _transformComponents[0].OwnerId = 0;
    _transformComponents[0].Active = true;
}

uint32_t Memory::CreateEntity(const std::string& name) {
    for (uint32_t i=0; i<_entities.size(); i++) {
        if (!_entities[i].Active) {
            _entities[i].Activate(name);
            return i;
        }
    }
    return 0; // return Id of default entity
}

template<typename T, typename ... Args>
uint32_t Memory::CreateComponent(uint32_t entityId, Args&&... args) {
    static_assert(std::is_base_of_v<Component, T>, "T must be derived from Component");

    if constexpr (std::is_same_v<T, TransformComponent>) {
        for (uint32_t i=0; i<_transformComponents.size(); i++) {
            if (!_transformComponents[i].Active) {
                _entities[entityId].TransformID = i;
                _transformComponents[i].Activate(entityId, std::forward<Args>(args)...);
                return i;
            }
        }
    }
    else {
        static_assert(!sizeof(T*), "No memory arena defined for this Component type.");
    }

    // assign default transform component to entity
    _entities[entityId].TransformID = 0;
    return 0;
}

template uint32_t Memory::CreateComponent<TransformComponent>(uint32_t entityId);