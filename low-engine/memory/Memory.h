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

#include "../log/Log.h"
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
            std::vector<std::type_index> Dependencies;
        };

        /**
         * @brief Default constructor.
         *
         * Initializes a new Memory manager for the ECS system.
         */
        Memory();

        /**
         * @brief Copy constructor.
         *
         * Creates a new Memory manager by copying the state of another one.
         * @param other The Memory instance to copy from.
         */
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

        template<typename T>
        void DestroyEntity(T* entity) {
            if (entity == nullptr) {
                _log->error("Cannot destroy a null entity");
                return;
            }

            size_t entityId = entity->Id;
            if (entityId >= _entities.size()) {
                _log->error("Entity id is out of range");
                return;
            }

            // Remove all components associated with the entity
            for (const auto& typeInfo: _typeInfos) {
                auto typeIndex = typeInfo.first;
                if (_components.contains(typeIndex)) {
                    _components[typeIndex]->DestroyComponent(entityId);
                }
            }

            // Remove the entity itself
            _entities[entityId].reset();
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
                if (entity != nullptr && entity->Name == name) {
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
                ti.Dependencies = T::Dependencies();
            }

            if (entityId >= _entities.size()) {
                _log->error("Entity id is out of range");
                return nullptr;
            }

            // checking dependencies
            const auto& typeInfo = _typeInfos[std::type_index(typeid(T))];
            for (const auto& dependency: typeInfo.Dependencies) {
                if (_components.find(dependency) == _components.end()) {
                    _log->error("Component {} is a dependency for {}, but it is not registered", DemangledTypeName(dependency),
                                DemangledTypeName(typeid(T)));
                    return nullptr;
                }
                if (_components[dependency]->GetComponentPtr(entityId) == nullptr) {
                    _log->error("Component {} is a dependency for {}, but it is not attached to Entity with id {}", DemangledTypeName(dependency),
                                DemangledTypeName(typeid(T)), entityId);
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

            _log->debug("Component {} created for Entity with id {}", DemangledTypeName(typeid(T)), entityId);

            return component;
        }

        template<typename T>
        bool IsComponentSafeToDestroy(size_t entityId) {
            // check if component is a dependency for any other component
            for (const auto& _component: _components) {
                const std::type_index& compType = _component.first;
                const std::unique_ptr<IComponentPool>& pool = _component.second;

                if (compType == std::type_index(typeid(T))) continue; // skip self-dependency

                auto componentPtr = pool->GetComponentPtr(entityId);
                if (componentPtr == nullptr) continue; // no component of this type attached to Entity

                const auto& typeInfo = _typeInfos[compType];
                if (std::find(typeInfo.Dependencies.begin(), typeInfo.Dependencies.end(), std::type_index(typeid(T))) != typeInfo.Dependencies.
                    end()) {
                    _log->debug("Component {} is a dependency for {}", DemangledTypeName(typeid(T)), DemangledTypeName(compType));
                    return false;
                }
            }
            return true;
        }

        /**
         * @brief Destroy Component of given type.
         * @tparam T Type of the Component that should be destroyed.
         * @param entityId Id of the Entity that owns Component.
         *
         * This method will remove Component from Entity and destroy it.
         */
        template<typename T>
        void DestroyComponent(size_t entityId) {
            auto typeIndex = std::type_index(typeid(T));
            if (_components.find(typeIndex) == _components.end()) {
                _log->warn("Component type {} not found", DemangledTypeName(typeid(T)));
                return;
            }
            _components[typeIndex]->DestroyComponent(entityId);
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
        /** @brief Counter for generating unique type IDs. */
        static inline unsigned int _nextTypeId = 0;

        /** @brief Collection of all entities in the system. */
        std::vector<std::unique_ptr<ECS::IEntity> > _entities;

        /** @brief Map of component pools indexed by their type. */
        std::unordered_map<std::type_index, std::unique_ptr<IComponentPool> > _components;

        /** @brief Map of type information for registered component types. */
        std::unordered_map<std::type_index, TypeInfo> _typeInfos;

        /**
         * @brief Get or create a component pool for a specific type.
         *
         * @tparam T The component type for the pool.
         * @return Reference to the component pool for type T.
         */
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
