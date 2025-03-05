#include "SceneManager.h"

LowEngine::SceneManager::SceneManager(): _scenes(), _currentSceneIndex(0) {
    _scenes.resize(1);
    _scenes[0].InitAsDefault();
}

LowEngine::Scene& LowEngine::SceneManager::CreateScene(const std::string& name) {
    Scene scene(name);
    scene.Active = true;
    _scenes.push_back(std::move(scene));

    return _scenes.back();
}

bool LowEngine::SceneManager::SelectScene(uint32_t index) {
    if (index < _scenes.size() && _scenes[index].Active) {
        _currentSceneIndex = index;
        return true;
    }
    return false;
}

bool LowEngine::SceneManager::SelectScene(const std::string& name) {
    for (uint32_t i = 0; i < _scenes.size(); i++) {
        if (_scenes[i].Active && _scenes[i].Name == name) {
            _currentSceneIndex = i;
            return true;
        }
    }
    return false;
}

bool LowEngine::SceneManager::SelectScene(const Scene& scene) {
    for (uint32_t i = 0; i < _scenes.size(); i++) {
        if (&_scenes[i] == &scene) {
            _currentSceneIndex = i;
            return true;
        }
    }
    return false;
}

LowEngine::Scene& LowEngine::SceneManager::GetCurrentScene() {
    return _scenes[_currentSceneIndex];
}

const LowEngine::Scene& LowEngine::SceneManager::GetCurrentScene() const {
    return _scenes[_currentSceneIndex];
}

void LowEngine::SceneManager::DestroyAll() {
    for (auto& scene : _scenes) {
        scene.Destroy();
    }
    _scenes.clear();
}
