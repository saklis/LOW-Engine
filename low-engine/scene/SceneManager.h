#pragma once

#include <string>
#include <array>

#include "Scene.h"

namespace LowEngine {
    class SceneManager {
    public:
        SceneManager();
        ~SceneManager() = default;

        uint32_t CreateScene(const std::string& name);
        void SetAsCurrent(uint32_t sceneId);
        Scene& GetCurrent();

        Scene& operator[](uint32_t sceneId);
    protected:
        std::array<Scene, 10> _scenes;
        uint32_t _currentSceneId = 0;
    };
}
