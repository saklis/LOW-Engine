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
    class Memory {
    public:
        struct TypeInfo {
            std::string Name = "";
            unsigned int Id = 0;
            std::type_index TypeIndex = std::type_index(typeid(void));
            size_t Size = 0;
        };

        Memory();
        Memory(Memory const& other);

        template<typename T>
        T* CreateEntity(const std::string& name) {
            auto entity = std::make_unique<T>(this);
            entity->Activate(name);
            _entities.push_back(std::move(entity));
            _entities.back()->Id = _entities.size() - 1;
            return static_cast<T*>(_entities.back().get());
        }

        template<typename T>
        T* GetEntity(size_t entityId) {
            return static_cast<T*>(_entities[entityId].get());
        }

        template<typename T>
        T* FindEntity(const std::string& name) {
            for (const auto& entity: _entities) {
                if (entity->Name == name) {
                    return static_cast<T*>(entity.get());
                }
            }
            return nullptr;
        }

        std::vector<std::unique_ptr<ECS::IEntity> >* GetAllEntities() {
            return &_entities;
        }

        template<typename T, typename... Args>
        T* CreateComponent(size_t entityId, Args&&... args) {
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
            component->EntityId = entityId;
            component->Active = true;
            component->Initialize();

            return component;
        }

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

        template<typename T>
        T* GetComponent(size_t entityId) {
            auto typeIndex = std::type_index(typeid(T));
            return static_cast<T*>(GetComponent(entityId, typeIndex));
        }

        template<typename T>
        std::vector<std::aligned_storage_t<sizeof(T), alignof(T)> >* GetAllComponents() {
            ComponentPool<T> pool = GetOrCreatePool<T>();
            return pool.GetComponentStorage();
        }

        template<typename T, typename Callback>
        void ForEachComponent(Callback&& callback) {
            ComponentPool<T>& pool = GetOrCreatePool<T>();
            pool.ForEachComponent(std::forward<Callback>(callback));
        }

        void UpdateAllComponents(float deltaTime);

        void CollectSprites(std::vector<Sprite>& sprites);

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
