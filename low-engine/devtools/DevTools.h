#pragma once

#include <format>


#include <imgui-SFML.h>
#include <imgui.h>

#include "Game.h"

namespace LowEngine {
    class DevTools {
    public:
        static void Display(Game& game);

    protected:
        static inline ECS::Entity* _selectedEntity = nullptr;
        static inline void* _selectedComponent = nullptr;
        static inline std::type_index _selectedComponentType = typeid(void);

        static void DisplayWorldOutliner(Scene& scene);

        static void DisplayProperties(Scene& scene);
        static void DisplayTransformComponentProperties(Scene& scene, ECS::Entity& entity);
        static void DisplayAnimatedSpriteComponentProperties(Scene& scene, ECS::Entity& entity);

        static const char* DemangledTypeName(const std::type_index& type);
        static std::string InsertSpaces(const std::string& str);
    };
}
