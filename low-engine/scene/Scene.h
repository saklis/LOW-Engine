#pragma once

#include <string>
#include <cinttypes>

#include "memory/Memory.h"

namespace LowEngine::Scene {
    class Scene {
    public:
        std::string Name;

        explicit Scene(std::string name);

        void Update();

        uint32_t AddEntity(const std::string& name = "Entity");

        template <typename T, typename... Args>
        uint32_t AddComponent(uint32_t entityId, Args&&... args);

    protected:
        Memory::Memory _memory;
    };
}
