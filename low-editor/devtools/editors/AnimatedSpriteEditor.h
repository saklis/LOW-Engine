#pragma once

#include <cstddef>

namespace LowEngine {
    class Scene;

    namespace Editors {
        void AddAnimatedSpriteComponent(Scene* scene, std::size_t entityId);
        void DrawAnimatedSpriteEditor(Scene* scene, std::size_t entityId);
    }

}