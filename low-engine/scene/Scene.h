#pragma once

#include <string>

#include "SFML/Graphics/RenderWindow.hpp"

#include "memory/Memory.h"
#include "ecs/ECSHeaders.h"

namespace LowEngine {
    class Scene {
    public:
        enum class SpriteSortingMethod {
            None,
            Layers,
            YAxisIncremental
        };

        bool Active = false;
        bool IsTemporary = false;
        bool IsPaused = false;

        std::string Name;

        Scene() = default;
        Scene(Scene const& other);

        explicit Scene(const std::string& name);

        void InitAsDefault();

        void Update(float deltaTime);

        void Draw(sf::RenderWindow& window);

        ECS::Entity* AddEntity(const std::string& name = "Entity");

        ECS::Entity* GetEntity(unsigned int entityId);
        ECS::Entity* FindEntity(const std::string& name);
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

        bool SetCurrentCamera(size_t entityId);
        void SetWindowSize(sf::Vector2f windowSize);

        void SetSpriteSorting(SpriteSortingMethod method);

        void Destroy();

    protected:
        size_t _cameraEntityId = Config::MAX_SIZE;
        SpriteSortingMethod _spriteSortingMethod = SpriteSortingMethod::None;
        Memory::Memory _memory;
    };
}
