#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include "scene/Scene.h"

namespace LowEngine {
    class Game;

    class SceneManager {
    public:
        SceneManager();

        ~SceneManager() = default;

        LowEngine::Scene& CreateScene(const std::string& name);

        // Select a scene as 'current'
        bool SelectScene(unsigned int index);

        bool SelectScene(const std::string& name);

        bool SelectScene(const LowEngine::Scene& scene);

        // Get the current active scene
        LowEngine::Scene& GetCurrentScene();

        const LowEngine::Scene& GetCurrentScene() const;

        void DestroyAll();

    protected:
        std::vector<LowEngine::Scene> _scenes;
        unsigned int _currentSceneIndex;
    };
}
