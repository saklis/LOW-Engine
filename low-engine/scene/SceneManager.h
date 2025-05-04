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

        Scene& CreateScene(const std::string& name);

        unsigned CreateTemporarySceneFromCurrent();

        // Select a scene as 'current'
        bool SelectScene(unsigned int index);

        bool SelectScene(const std::string& name);

        bool SelectScene(const LowEngine::Scene& scene);

        // Get the current active scene
        Scene& GetCurrentScene();

        const Scene& GetCurrentScene() const;

        void DestroyCurrentScene();
        void DestroyAll();

    protected:
        std::vector<std::unique_ptr<Scene>> _scenes;
        unsigned int _currentSceneIndex;
    };
}
