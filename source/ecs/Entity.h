#pragma once

#include <cstdint>
#include <string>

class Entity {
public:
    bool Active = false;
    uint32_t Id = 0;
    std::string Name;

    Entity();

    void ActivateAsDefault();
    void Activate(const std::string& name);

protected:
    static uint32_t _nextId;
};
