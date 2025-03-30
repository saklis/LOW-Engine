#pragma once

#include <vector>
#include <unordered_map>

#include "Log.h"

namespace LowEngine::Memory {
    class IComponentPool {
        public:
            virtual ~IComponentPool() = default;

            virtual void* GetComponentPtr(size_t entityId) = 0;
            virtual void Update(float deltaTime) = 0;
        };

        template<typename T>
        class ComponentPool : public IComponentPool {
        protected:
            std::vector<std::aligned_storage_t<sizeof(T), alignof(T)>> Storage;

            std::unordered_map<size_t, size_t> IndexMap;
            std::unordered_map<size_t, size_t> ReverseMap;

        public:
            static const size_t MAX_COMPONENTS = 1000;

            explicit ComponentPool(size_t capacity = MAX_COMPONENTS) {
                Storage.reserve(capacity);
            }

            template<typename... Args>
            T* CreateComponent(size_t entityId, Args&&... args) {
                if (IndexMap.find(entityId) != IndexMap.end()) {
                    _log->error("Component pool: Component {} already exists for entity id {}.", typeid(T).name(), entityId);
                    return nullptr;
                }

                // Storage.emplace_back(std::forward<Args>(args)...);
                // size_t index = Storage.size() - 1;
                //
                // IndexMap[entityId] = index;
                // ReverseMap[index] = entityId;
                //
                // return reinterpret_cast<T*>(&Storage.back());

                // get index to create component
                size_t index = Storage.size();
                if (index >= Storage.capacity()) {
                    // error handling or resize storage here
                    return nullptr;
                }

                // placement-new to initialize memory
                Storage.emplace_back();
                T* component = new(&Storage.back()) T(std::forward<Args>(args)...);

                // map entityId to component index
                IndexMap[entityId] = index;
                ReverseMap[index] = entityId;

                return component; // now initialized properly
            }

            void DestroyComponent(size_t entityId) {
                auto it = IndexMap.find(entityId);
                if (it == IndexMap.end()) {
                    return; // component not found
                }

                size_t removedIndex = it->second;
                size_t lastIndex = Storage.size() - 1;

                // swamp component to remove (index) with the last one
                if (removedIndex != lastIndex) {
                    std::swap(Storage[removedIndex], Storage[lastIndex]);

                    size_t swappedEntityId = ReverseMap[lastIndex];
                    IndexMap[swappedEntityId] = removedIndex;
                    ReverseMap[removedIndex] = swappedEntityId;
                }

                Storage.pop_back();
                IndexMap.erase(it);
                ReverseMap.erase(lastIndex);
            }

            void* GetComponentPtr(size_t entityId) override {
                auto it = IndexMap.find(entityId);
                if (it == IndexMap.end()) {
                    return nullptr;
                }
                return &Storage[it->second];
            }

            std::vector<std::aligned_storage_t<sizeof(T), alignof(T)>>* GetComponentStorage() {
                return &Storage;
            }

            void Update(float deltaTime) override {
                for (auto& component : Storage) {
                    reinterpret_cast<T*>(&component)->Update(deltaTime);
                }
            }
        };
}