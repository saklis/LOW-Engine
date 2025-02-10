#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <vector>

namespace LowEngine::ECS {
    class Entity {
    public:
        bool Active = false;
        uint32_t Id = 0;
        std::string Name;

        uint32_t TransformId = 0;
        uint32_t SpriteId = 0;
        std::vector<uint32_t> CustomComponents;

        Entity();

        void InitAsDefault();
        void Activate(const std::string& name);

    protected:
        static uint32_t _nextId;
    };
}
