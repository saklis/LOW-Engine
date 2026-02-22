#pragma once

#include <cstddef>

namespace LowEngine {
    class Scene;

    namespace Editors {
        void AddSpriteComponent(Scene* scene, std::size_t entityId);
        void DrawSpriteEditor(Scene* scene, std::size_t entityId);
    }

}