#include "SceneManager.h"

namespace LowEngine {
    SceneManager::SceneManager(): _scenes(), _currentSceneIndex(0) {
        _scenes.resize(1);
        _scenes[0] = std::move(std::make_unique<Scene>());
        _scenes[0]->InitAsDefault();
    }

    Scene& SceneManager::CreateScene(const std::string& name) {
        auto scene = std::make_unique<Scene>(name);
        scene->Active = true;
        _scenes.push_back(std::move(scene));

        _log->info("New scene created: '{}'", name);

        return *_scenes.back();
    }

    size_t SceneManager::CreateTemporarySceneFromCurrent() {
        if (_scenes.empty()) {
            _log->info("No scene to copy");
            return Config::MAX_SIZE;
        }

        // Copy‐construct from the current scene:
        Scene* current = _scenes[_currentSceneIndex].get();
        auto clone = std::make_unique<Scene>(*current);

        clone->Active = true;
        clone->IsTemporary = true;

        _scenes.push_back(std::move(clone));
        return _scenes.size() - 1;
    }

    bool SceneManager::SelectScene(unsigned int index) {
        if (index < _scenes.size() && _scenes[index]->Active) {
            _currentSceneIndex = index;
            return true;
        }
        return false;
    }

    bool SceneManager::SelectScene(const std::string& name) {
        for (unsigned int i = 0; i < _scenes.size(); i++) {
            if (_scenes[i]->Active && _scenes[i]->Name == name) {
                _currentSceneIndex = i;
                return true;
            }
        }
        return false;
    }

    bool SceneManager::SelectScene(const Scene& scene) {
        for (unsigned int i = 0; i < _scenes.size(); i++) {
            if (_scenes[i].get() == &scene) {
                _currentSceneIndex = i;

                _log->info("Scene selected: '{}'", scene.Name);

                return true;
            }
        }
        return false;
    }

    Scene& SceneManager::GetCurrentScene() {
        return *_scenes[_currentSceneIndex];
    }

    const Scene& SceneManager::GetCurrentScene() const {
        return *_scenes[_currentSceneIndex];
    }

    void SceneManager::DestroyCurrentScene() {
        if (_scenes.empty()) {
            _log->warn("No current scene to destroy");
            return;
        }

        _scenes[_currentSceneIndex]->Destroy();
        _scenes.erase(_scenes.begin() + _currentSceneIndex);

        if (_scenes.empty()) {
            _currentSceneIndex = 0;
            _log->debug("All scenes destroyed; default scene set as current scene");
        } else if (_currentSceneIndex >= _scenes.size()) {
            _currentSceneIndex = _scenes.size() - 1;
            _log->debug("Current scene destroyed; switching to scene index {}", _currentSceneIndex);
        } else {
            _log->debug("Current scene destroyed; switching to scene index {}", _currentSceneIndex);
        }
    }

    void SceneManager::DestroyAll() {
        for (auto& scene: _scenes) {
            scene->Destroy();
        }
        _scenes.clear();

        _log->info("All scenes destroyed");
    }
}
