#pragma once

#include <string>

#include "memory/Memory.h"

namespace LowEngine {
    class Scene {
    public:
        bool Active = false;
        std::string Name;

        Scene() = default;

        explicit Scene(const std::string& name);

        void InitAsDefault();

        void Update(float deltaTime);

        void Draw(sf::RenderWindow& window);

        unsigned int AddEntity(const std::string& name = "Entity");

        std::vector<ECS::Entity>& GetEntities();

        template<typename T, typename... Args>
        T* AddComponent(unsigned int entityId, Args&&... args) {
            return _memory.CreateComponent<T>(entityId, std::forward<Args>(args)...);
        }

        template<typename T>
        T* GetComponent(unsigned int entityId) {
            return _memory.GetComponent<T>(entityId);
        }

        void* GetComponent(unsigned int entity_id, std::type_index typeIndex);

        void Destroy();

    protected:
        Memory::Memory _memory;
    };
}
