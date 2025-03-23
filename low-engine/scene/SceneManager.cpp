#include "SceneManager.h"

namespace LowEngine {
    SceneManager::SceneManager(): _scenes(), _currentSceneIndex(0) {
        _scenes.resize(1);
        _scenes[0].InitAsDefault();
    }

    Scene& SceneManager::CreateScene(const std::string& name) {
        Scene scene(name);
        scene.Active = true;
        _scenes.push_back(std::move(scene));

        return _scenes.back();
    }

    bool SceneManager::SelectScene(unsigned int index) {
        if (index < _scenes.size() && _scenes[index].Active) {
            _currentSceneIndex = index;
            return true;
        }
        return false;
    }

    bool SceneManager::SelectScene(const std::string& name) {
        for (unsigned int i = 0; i < _scenes.size(); i++) {
            if (_scenes[i].Active && _scenes[i].Name == name) {
                _currentSceneIndex = i;
                return true;
            }
        }
        return false;
    }

    bool SceneManager::SelectScene(const Scene& scene) {
        for (unsigned int i = 0; i < _scenes.size(); i++) {
            if (&_scenes[i] == &scene) {
                _currentSceneIndex = i;
                return true;
            }
        }
        return false;
    }

    Scene& SceneManager::GetCurrentScene() {
        return _scenes[_currentSceneIndex];
    }

    const Scene& SceneManager::GetCurrentScene() const {
        return _scenes[_currentSceneIndex];
    }

    void SceneManager::DestroyAll() {
        for (auto& scene: _scenes) {
            scene.Destroy();
        }
        _scenes.clear();
    }
}
