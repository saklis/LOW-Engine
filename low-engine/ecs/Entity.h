#pragma once

#include <cstdint>
#include <string>

class Entity {
public:
    bool Active = false;
    uint32_t Id = 0;
    std::string Name;

    uint32_t TransformID = 0;

    Entity();

    void ActivateAsDefault();
    void Activate(const std::string& name);

protected:
    static uint32_t _nextId;
};
