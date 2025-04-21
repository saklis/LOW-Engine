#pragma once

#include <vector>
#include <unordered_map>

#include "Config.h"
#include "Log.h"
#include "graphics/Sprite.h"

namespace LowEngine::Memory {
    class Memory;

    class IComponentPool {
    public:
        virtual ~IComponentPool() = default;

        virtual void* GetComponentPtr(size_t entityId) = 0;

        virtual void Update(float deltaTime) = 0;

        virtual void CollectSprites(std::vector<Sprite>& sprites) = 0;
    };

    template<typename T>
    class ComponentPool : public IComponentPool {
    public:
        explicit ComponentPool(size_t capacity = Config::MAX_COMPONENTS) {
            Storage.reserve(capacity);
        }

        ~ComponentPool() override {
            for (auto& component: Storage) {
                reinterpret_cast<T*>(&component)->~T();
            }
        };

        template<typename... Args>
        T* CreateComponent(Memory* memory, size_t entityId, Args&&... args) {
            if (IndexMap.find(entityId) != IndexMap.end()) {
                _log->error("Component pool: Component {} already exists for entity id {}.", typeid(T).name(), entityId);
                return nullptr;
            }

            // get index to create component
            size_t index = Storage.size();
            if (index >= Storage.capacity()) {
                // error handling or resize storage here
                return nullptr;
            }

            // placement-new to initialize memory
            Storage.emplace_back();
            T* component = new(&Storage.back()) T(memory, std::forward<Args>(args)...);

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
            reinterpret_cast<T*>(&Storage[removedIndex])->~T();

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

        std::vector<std::aligned_storage_t<sizeof(T), alignof(T)> >* GetComponentStorage() {
            return &Storage;
        }

        T* GetComponentFromStorage(std::aligned_storage_t<sizeof(T), alignof(T)>& storage) {
            return reinterpret_cast<T*>(&storage);
        }

        template<typename Callback>
        void ForEachComponent(Callback&& callback) {
            for (auto& storage: Storage) {
                T* component = reinterpret_cast<T*>(&storage);
                callback(*component);
            }
        }

        void Update(float deltaTime) override {
            for (auto& component: Storage) {
                reinterpret_cast<T*>(&component)->Update(deltaTime);
            }
        }

        void CollectSprites(std::vector<Sprite>& sprites) override {
            for (auto& storage : Storage) {
                T* component = reinterpret_cast<T*>(&storage);
                Sprite* sprite = component->Draw();
                if (sprite != nullptr) {
                    sprites.emplace_back(*sprite);
                }
            }
        }

    protected:
        std::vector<std::aligned_storage_t<sizeof(T), alignof(T)> > Storage;

        std::unordered_map<size_t, size_t> IndexMap;
        std::unordered_map<size_t, size_t> ReverseMap;
    };
}
