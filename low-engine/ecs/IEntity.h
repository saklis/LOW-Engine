#pragma once

namespace LowEngine::ECS {
    class IEntity {
    public:
        virtual ~IEntity() = default;

        bool Active = false;
        size_t Id = 0;
        std::string Name;

        virtual bool HasComponent(const std::type_index& typeIndex) = 0;
    protected:
        IEntity() = default;
    };
}
