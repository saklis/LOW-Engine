#pragma once

#include <cstdint>
#include <string>
#include <typeindex>
#include <vector>
#include <unordered_map>

#ifdef _MSC_VER
       #include <cstdlib>
#else
#include <cxxabi.h>
#endif

#include <stack>

#include "Log.h"
#include "ecs/IEntity.h"
#include "memory/ComponentPool.h"
#include "graphics/Sprite.h"

namespace LowEngine::Memory {
    /**
     * @brief Manages entities and components within an entity-component system (ECS).
     *
     * The Memory class provides mechanisms to create, manage, and access entities and their components.
     * It supports component pools, type information tracking, and dependency checks
     * during component creation.
     */
    class Memory {
    public:
        /**
         * @brief Provides metadata and type information for objects.
         *
         * Specificaly used to keep track of Component Types.
         */
        struct TypeInfo {
            std::string Name = "";
            unsigned int Id = 0;
            std::type_index TypeIndex = std::type_index(typeid(void));
            size_t Size = 0;
        };

        Memory();

        Memory(Memory const& other);

        /**
         * @brief Creates new Entity object.
         * @tparam T Type of Entity. Must extend IEntity
         * @param name Name of thse new Entity
         * @return Pointer to new Entity. Returns nullptr in case of error.
         */
        template<typename T>
        T* CreateEntity(const std::string& name) {
            std::unique_ptr<T> entity{new(std::nothrow) T(this)};
            if (entity == nullptr) {
                _log->error("Failed to create entity of type {}", typeid(T).name());
                return nullptr;
            }
            entity->Activate(name);
            _entities.push_back(std::move(entity));
            _entities.back()->Id = _entities.size() - 1;
            return static_cast<T*>(_entities.back().get());
        }

        /**
         * @brief Retrieves Entity by provided Id
         * @tparam T Type of Entity. Must extend IEntity
         * @param entityId Id of the Entity to retrieve.
         * @return Pointer to Entity. Returns nullptr if Entity with Id doesn't exists.
         */
        template<typename T>
        T* GetEntity(size_t entityId) {
            if (entityId >= _entities.size()) {
                return nullptr;
            }

            return static_cast<T*>(_entities[entityId].get());
        }

        /**
         * @brief Retrieves Entity by its name.
         *
         * If there's multiple Entities with the same name, the first one found will be retrieved.
         * @tparam T Type of Entity. Must extend IEntity
         * @param name Name od the Entity to retrieve.
         * @return Pointer to Entity. Returns nullptr if Entity with Name doesn't exist.
         */
        template<typename T>
        T* FindEntity(const std::string& name) {
            for (const auto& entity: _entities) {
                if (entity->Name == name) {
                    return static_cast<T*>(entity.get());
                }
            }
            return nullptr;
        }

        /**
         * @brief Retrieve entire collection of Entities.
         *
         * Method created to be used in DevTools.
         * Be extra careful if using in Game Logic.
         * @return Pointer to entire collection of Entities.
         */
        std::vector<std::unique_ptr<ECS::IEntity> >* GetAllEntities() {
            return &_entities;
        }

        /**
         * @brief Create new component and assigne it to Entity with provided Id.
         * @tparam T Type of Component. Must extend IComponent
         * @tparam Args Template arguments that will be forwarded to Component's c-tor
         * @param entityId Id of the Entity that should have new Component attached.
         * @param args List of arguments that should be forwarded to Component's c-tor
         * @return Pointer to new componentn. Returns nullptr in case of error.
         */
        template<typename T, typename... Args>
        T* CreateComponent(size_t entityId, Args&&... args) {
            // register type
            if (!_typeInfos.contains(std::type_index(typeid(T)))) {
                TypeInfo& ti = _typeInfos[std::type_index(typeid(T))];
                ti.Name = typeid(T).name();
                ti.Id = _nextTypeId++;
                ti.TypeIndex = std::type_index(typeid(T));
                ti.Size = sizeof(T);
            }

            if (entityId >= _entities.size()) {
                _log->error("Entity id is out of range");
                return nullptr;
            }

            // checking dependencies
            for (const auto& depType: T::Dependencies()) {
                if (!_entities[entityId]->HasComponent(depType)) {
                    _log->error("Component {} is required by {} but not found.", depType.name(), typeid(T).name());
                    return nullptr;
                }
            }

            ComponentPool<T>& pool = GetOrCreatePool<T>();
            T* component = pool.CreateComponent(this, entityId, std::forward<Args>(args)...);
            if (component != nullptr) {
                component->EntityId = entityId;
                component->Active = true;
                component->Initialize();
            }

            return component;
        }

        /**
         * @brief Retrieve component of requested type.
         * @param entityId Id of the Entity that component is attached to.
         * @param typeIndex Type of the component.
         * @return Pointer to Component. Returns nullptr if Component was not found.
         */
        void* GetComponent(size_t entityId, const std::type_index& typeIndex) {
            if (_entities.size() <= entityId) {
                _log->error("Entity id is out of range");
                return nullptr;
            }
            if (_components.find(typeIndex) == _components.end()) {
                return nullptr;
            }
            return _components[typeIndex]->GetComponentPtr(entityId);
        }

        /**
         * @brief Retrieve component of requested type.
         * @tparam T Type of the component.
         * @param entityId Id of the Entity that component is attached to.
         * @return Pointer to Component. Returns nullptr if Component was not found.
         */
        template<typename T>
        T* GetComponent(size_t entityId) {
            auto typeIndex = std::type_index(typeid(T));
            return static_cast<T*>(GetComponent(entityId, typeIndex));
        }

        /**
         * @brief Call function for all Components of particular type.
         * @tparam T Type of Component
         * @tparam Callback Type of a callback to be executed.
         * @param callback Reference to a function that will be called.
         */
        template<typename T, typename Callback>
        void ForEachComponent(Callback&& callback) {
            ComponentPool<T>& pool = GetOrCreatePool<T>();
            pool.ForEachComponent(std::forward<Callback>(callback));
        }

        /**
         * @brief Call Update function of all Components.
         * @param deltaTime Time passed since last call, in seconds.
         */
        void UpdateAllComponents(float deltaTime);

        /**
         * @brief Check all Components in search of Sprites to draw.
         *
         * Sprites will be added to refered collection.
         * @param[out] sprites Reference to collection that will be filled with Sprites that needs to be drawn.
         */
        void CollectSprites(std::vector<Sprite>& sprites);

        /**
         * @brief Remove all Entities and Component.
         */
        void Destroy();

    protected:
        static inline unsigned int _nextTypeId = 0;

        std::vector<std::unique_ptr<ECS::IEntity> > _entities;
        std::unordered_map<std::type_index, std::unique_ptr<IComponentPool> > _components;
        std::unordered_map<std::type_index, TypeInfo> _typeInfos;

        template<typename T>
        ComponentPool<T>& GetOrCreatePool() {
            auto typeIdx = std::type_index(typeid(T));
            auto it = _components.find(typeIdx);
            if (it == _components.end()) {
                auto newPool = std::make_unique<ComponentPool<T> >();
                auto ptr = newPool.get();
                _components[typeIdx] = std::move(newPool);
                return *ptr;
            }
            return *static_cast<ComponentPool<T>*>(it->second.get());
        }
    };
}
