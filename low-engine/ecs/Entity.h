#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace LowEngine::ECS {
    class Entity {
    public:
        bool Active = false;
        unsigned int Id = 0;
        std::string Name;

        Entity();

        void InitAsDefault();

        void Activate(const std::string& name);

        void AddComponent(const std::type_index& typeIndex, unsigned int componentId);
        int GetComponent(const std::type_index& typeIndex);
        std::vector<std::type_index> GetComponentTypes();

    protected:
        static unsigned int _nextId;

        std::unordered_map<std::type_index, std::vector<unsigned int>> _components;
    };
}
