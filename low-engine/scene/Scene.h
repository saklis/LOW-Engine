#pragma once

#include <string>
#include <cinttypes>

#include "memory/Memory.h"

class Scene {
public:
    std::string Name;

    explicit Scene(std::string  name);

    uint32_t AddEntity(const std::string& name = "Entity");

    template <typename T, typename... Args>
    uint32_t AddComponent(uint32_t entityId, Args&&... args);

protected:
    Memory _memory;
};
