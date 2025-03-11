#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

#include "ComponentView.h"
#include "ECS/ECSHeaders.h"


namespace LowEngine::Memory {
    class Memory {
    public:
        Memory();

        uint32_t CreateEntity(const std::string& name);

        ECS::Entity& GetEntity(uint32_t entityId);

        const ECS::Entity& GetEntity(uint32_t entityId) const;

        std::vector<ECS::Entity>& GetEntities();

        template<typename T, typename... Args>
        T* CreateComponent(uint32_t entityId, Args&&... args) {
            auto& byteVector = _components[typeid(T).name()];
            size_t currentSize = byteVector.size();
            byteVector.resize(currentSize + sizeof(T));

            void* memory = byteVector.data() + currentSize;
            T* component = new(memory) T(); // placement new to construct object in target memory

            uint32_t componentId = currentSize / sizeof(T);
            _entities[entityId].AddComponent(typeid(T).name(), componentId);

            component->EntityId = entityId;
            component->Active = true;
            component->Initialize();

            size_t offset = componentId * sizeof(T);
            return reinterpret_cast<T*>(byteVector.data() + offset);
        }

        template<typename T>
        T* GetComponent(uint32_t entityId) {
            std::string typeName = typeid(T).name();

            int32_t componentId = _entities[entityId].GetComponent(typeName);

            if (componentId >= 0) {
                auto& byteVector = _components[typeName];
                size_t offset = componentId * sizeof(T);
                if (offset + sizeof(T) > byteVector.size()) {
                    throw std::runtime_error("Component Id is out of range for type: " + typeName);
                }

                return reinterpret_cast<T*>(byteVector.data() + offset);
            }

            return nullptr;
        }

        template<typename T>
        ComponentView<T> GetAllComponents() {
            const std::string typeName = typeid(T).name();
            auto it = _components.find(typeName);
            if (it == _components.end()) {
                return {nullptr, 0}; // Empty view
            }

            auto& byteVector = it->second;

            // Ensure the memory can be correctly interpreted as T
            if (byteVector.size() % sizeof(T) != 0) {
                throw std::runtime_error(
                    "Memory block size is not a multiple of the component size for type: " + typeName);
            }

            return {byteVector.data(), byteVector.size() / sizeof(T)};
        }

        void Destroy();

    protected:
        std::vector<ECS::Entity> _entities;
        std::unordered_map<std::string, std::vector<std::byte> > _components;
    };
}
