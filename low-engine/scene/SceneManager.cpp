#include "SceneManager.h"

uint32_t LowEngine::SceneManager::CreateScene(const std::string& name) {
    for (uint32_t i = 0; i < _scenes.size(); i++) {
        if (_scenes[i].Active == false) {
            _scenes[i].Name = name;
            _scenes[i].Active = true;
            return i;
        }
    }
    return 0; // return Id of default scene
}

void LowEngine::SceneManager::SetAsCurrent(uint32_t sceneId) {
    if (sceneId < _scenes.size()) {
        _currentSceneId = sceneId;
    } else {
        _currentSceneId = 0;
    }
}

LowEngine::Scene& LowEngine::SceneManager::GetCurrent() {
    return _scenes[_currentSceneId];
}

LowEngine::SceneManager::SceneManager() {
    _scenes[0].InitAsDefault();
}

LowEngine::Scene& LowEngine::SceneManager::operator[](uint32_t sceneId) {
    if (sceneId < _scenes.size()) {
        return _scenes[sceneId];
    }
    return _scenes[0];
}
