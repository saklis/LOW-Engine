#include "Memory.h"

LowEngine::Memory::Memory::Memory() {
    _entities[0].InitAsDefault();

    // initialize default components
    _transformComponents[0].InitAsDefault();
    _spriteComponents[0].InitAsDefault();
}

uint32_t LowEngine::Memory::Memory::CreateEntity(const std::string& name) {
    for (uint32_t i = 0; i < _entities.size(); i++) {
        if (!_entities[i].Active) {
            _entities[i].Activate(name);
            return i;
        }
    }
    return 0; // return default entity
}
