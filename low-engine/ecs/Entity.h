#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <unordered_map>
#include <vector>

namespace LowEngine::ECS {
    class Entity {
    public:
        bool Active = false;
        uint32_t Id = 0;
        std::string Name;

        int32_t TransformId = -1;
        int32_t SpriteId = -1;
        std::vector<uint32_t> CustomComponents;

        Entity();

        void InitAsDefault();

        void Activate(const std::string& name);

        void AddComponent(const std::string& typeName, uint32_t componentId);
        uint32_t GetComponent(const std::string& typeName);

    protected:
        static uint32_t _nextId;

        std::unordered_map<std::string, std::vector<uint32_t>> _components;
    };
}
