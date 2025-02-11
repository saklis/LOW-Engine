#pragma once

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "ecs/Component.h"

namespace LowEngine::Memory {
    class ComponentRegistry {
    public:
        template<typename T>
        void RegisterComponent(const std::string& typeName) {
            _factories[typeName] = []() -> std::unique_ptr<ECS::Component> {
                return std::make_unique<T>();
            };
        }

        bool IsComponentRegistered(const std::string& typeName) {
            return _factories.find(typeName) != _factories.end();
        }

        std::unique_ptr<ECS::Component> CreateComponent(const std::string& typeName) {
            auto it = _factories.find(typeName);
            if (it != _factories.end()) {
                return it->second();
            }

            throw std::runtime_error("Requested custom component type not found: " + typeName);
        }

    protected:
        using ComponentFactory = std::function<std::unique_ptr<ECS::Component>()>;
        std::unordered_map<std::string, ComponentFactory> _factories;
    };
}
