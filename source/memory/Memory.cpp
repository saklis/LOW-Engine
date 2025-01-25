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

uint32_t Memory::AddTransformComponent(uint32_t entityId) {
    for (uint32_t i=0; i<_transformComponents.size(); i++) {
        if (!_transformComponents[i].Active) {
            _transformComponents[i].Activate(entityId);
            return i;
        }
    }
    return 0; // return Id of default transform component
}
