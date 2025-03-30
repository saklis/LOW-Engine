#pragma once

#include <string>

#include "SFML/Graphics/RenderWindow.hpp"

#include "memory/Memory.h"
#include "ecs/Entity.h"

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

        ECS::Entity* AddEntity(const std::string& name = "Entity");

        ECS::Entity* GetEntity(unsigned int entityId);
        std::vector<std::unique_ptr<ECS::IEntity>>* GetEntities();

        template<typename T, typename... Args>
        T* AddComponent(unsigned int entityId, Args&&... args) {
            return _memory.CreateComponent<T>(entityId, std::forward<Args>(args)...);
        }

        template<typename T>
        T* GetComponent(unsigned int entityId) {
            return _memory.GetComponent<T>(entityId);
        }

        void* GetComponent(unsigned int entity_id, std::type_index typeIndex);

        bool SetCurrentCamera(int entityId);
        void SetWindowSize(sf::Vector2f windowSize);

        void Destroy();

    protected:
        int _cameraEntityId = -1;
        Memory::Memory _memory;
    };
}
