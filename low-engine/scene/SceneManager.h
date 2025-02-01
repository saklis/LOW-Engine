#pragma once

#include <string>
#include <stack>

#include "Scene.h"

namespace LowEngine::Scene {
    class SceneManager {
    public:
        void CreateScene(const std::string& name);
        Scene& GetCurrent();

        SceneManager() = default;
        ~SceneManager() = default;

    protected:
        std::stack<Scene> _scenes;
    };
}
