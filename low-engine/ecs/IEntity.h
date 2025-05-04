#pragma once

namespace LowEngine::Memory {
    class Memory;
}

namespace LowEngine::ECS {
    class IEntity {
    public:
        virtual ~IEntity() = default;

        virtual IEntity* Clone(Memory::Memory* newMemory) const = 0;

        bool Active = false;
        size_t Id = 0;
        std::string Name;

        virtual bool HasComponent(const std::type_index& typeIndex) = 0;
    protected:
        IEntity() = default;
    };
}
