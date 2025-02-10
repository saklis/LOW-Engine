#pragma once

#include <string>
#include <array>
#include <cstdint>

#include "scene/Scene.h"

namespace LowEngine {
    class SceneManager {
    public:
        SceneManager();
        ~SceneManager() = default;

        LowEngine::Scene& CreateScene(const std::string& name);
    protected:
        std::array<LowEngine::Scene, 10> _scenes;
    };
}
