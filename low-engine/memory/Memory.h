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

#include "ComponentView.h"
#include "ECS/ECSHeaders.h"

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

        void Update(float deltaTime);

        unsigned int CreateEntity(const std::string& name);

        ECS::Entity& GetEntity(unsigned int entityId);

        const ECS::Entity& GetEntity(unsigned int entityId) const;

        std::vector<ECS::Entity>& GetEntities();

        template<typename T, typename... Args>
        T* CreateComponent(unsigned int entityId, Args&&... args) {
            auto& byteVector = _components[std::type_index(typeid(T))];
            size_t currentSize = byteVector.size();
            byteVector.resize(currentSize + sizeof(T));

            void* memory = byteVector.data() + currentSize;
            T* component = new(memory) T(); // placement new to construct object in target memory

            if (!_typeInfos.contains(std::type_index(typeid(T)))) {
                TypeInfo& ti = _typeInfos[std::type_index(typeid(T))];
                ti.Name = typeid(T).name();
                ti.Id = _nextTypeId++;
                ti.TypeIndex = std::type_index(typeid(T));
                ti.Size = sizeof(T);
            }

            unsigned int componentId = currentSize / sizeof(T);
            _entities[entityId].AddComponent(std::type_index(typeid(T)), componentId);

            component->EntityId = entityId;
            component->Active = true;
            component->Initialize();

            size_t offset = componentId * sizeof(T);
            return reinterpret_cast<T*>(byteVector.data() + offset);
        }

        void* GetComponent(unsigned int entityId, const std::type_index& typeIndex) {
            auto it = _components.find(typeIndex);
            if (it == _components.end()) {
                return nullptr;
            }

            if (!_typeInfos.contains(typeIndex)) {
                return nullptr;
            }

            int componentId = _entities[entityId].GetComponent(typeIndex);
            if (componentId >= 0) {
                const auto& byteVector = it->second;

                size_t offset = componentId * _typeInfos[typeIndex].Size;
                if (offset + _typeInfos[typeIndex].Size > _components[typeIndex].size()) {
                    throw std::runtime_error(std::format("Component Id is out of range for type: {}",
                                                         typeIndex.name()));
                }

                return it->second.data() + offset;
            }

            return nullptr;
        }

        template<typename T>
        T* GetComponent(unsigned int entityId) {
            auto typeIndex = std::type_index(typeid(T));

            if (!_components.contains(typeIndex)) {
                return nullptr;
            }

            int componentId = _entities[entityId].GetComponent(typeIndex);

            if (componentId >= 0) {
                auto& byteVector = _components[typeIndex];
                size_t offset = componentId * sizeof(T);
                if (offset + sizeof(T) > byteVector.size()) {
                    throw std::runtime_error(std::format("Component Id is out of range for type: {}",
                                                         typeid(T).name()));
                }

                return reinterpret_cast<T*>(byteVector.data() + offset);
            }

            return nullptr;
        }

        template<typename T>
        ComponentView<T> GetAllComponents() {
            auto typeIndex = std::type_index(typeid(T));

            auto it = _components.find(typeIndex);
            if (it == _components.end()) {
                return {nullptr, 0}; // Empty view
            }

            auto& byteVector = it->second;

            // Ensure the memory can be correctly interpreted as T
            if (byteVector.size() % sizeof(T) != 0) {
                throw std::runtime_error(
                    std::format("Memory block size is not a multiple of the component size for type: {}",
                                typeid(T).name()));
            }

            return {byteVector.data(), byteVector.size() / sizeof(T)};
        }

        void Destroy();

    protected:
        static inline unsigned int _nextTypeId = 0;

        std::vector<ECS::Entity> _entities;
        std::unordered_map<std::type_index, std::vector<std::byte> > _components;
        std::unordered_map<std::type_index, TypeInfo> _typeInfos;
    };
}
