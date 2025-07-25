#include "SceneManager.h"

namespace LowEngine {
    SceneManager::SceneManager(): _scenes(), _currentSceneIndex(0) {
        _scenes.resize(1);
        _scenes[0] = std::move(std::make_unique<Scene>());
        _scenes[0]->InitAsDefault();
    }

    Scene* SceneManager::CreateEmptyScene(const std::string& name) {
        auto scene = std::make_unique<Scene>(name);
        scene->Initialized = true;
        _scenes.push_back(std::move(scene));

        _log->info("New scene created: '{}'", name);

        return _scenes.back().get();
    }

    Scene* SceneManager::CreateScene(const std::string& name) {
        auto newScene = CreateEmptyScene(name);

        if (newScene) {
            // camera entity
            auto cameraEntity = newScene->AddEntity("Default camera");
            if (cameraEntity) {
                if (auto tc = cameraEntity->AddComponent<ECS::TransformComponent>(); !tc) {
                    _log->error("Failed to add TransformComponent to camera entity in scene '{}'", name);
                    return nullptr;
                }
                if (auto camera = cameraEntity->AddComponent<ECS::CameraComponent>()) {
                    newScene->SetCurrentCamera(cameraEntity->Id);
                } else {
                    _log->error("Failed to add camera component to entity in scene '{}'", name);
                    return nullptr;
                }
            }
        }

        return newScene;
    }

    size_t SceneManager::CreateCopySceneFromCurrent(const std::string& nameSufix) {
        if (_scenes.empty()) {
            _log->warn("No scene to copy");
            return Config::MAX_SIZE;
        }

        // Copy‐construct from the current scene:
        Scene* current = _scenes[_currentSceneIndex].get();
        auto clone = std::make_unique<Scene>(*current);

        clone->Initialized = true;
        clone->IsTemporary = true;

        _scenes.push_back(std::move(clone));

        _log->info("Scene '{}' created as a copy of current scene '{}'", _scenes.back().get()->Name, current->Name);
        return _scenes.size() - 1;
    }

    bool SceneManager::SelectScene(size_t index) {
        if (index < _scenes.size() && _scenes[index]->Initialized) {
            _currentSceneIndex = index;
            return true;
        }
        return false;
    }

    bool SceneManager::SelectScene(const std::string& name) {
        for (unsigned int i = 0; i < _scenes.size(); i++) {
            if (_scenes[i]->Name == name && _scenes[i]->Initialized) {
                _currentSceneIndex = i;
                return true;
            }
        }
        return false;
    }

    bool SceneManager::SelectScene(const Scene& scene) {
        for (unsigned int i = 0; i < _scenes.size(); i++) {
            if (_scenes[i].get() == &scene && _scenes[i]->Initialized) {
                _currentSceneIndex = i;

                _log->info("Scene selected: '{}'", scene.Name);

                return true;
            }
        }
        return false;
    }

    Scene* SceneManager::GetCurrentScene() {
        return _scenes[_currentSceneIndex].get();
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
