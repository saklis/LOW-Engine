#include "SceneManager.h"

LowEngine::SceneManager::SceneManager(): _scenes() {
    _scenes[0].InitAsDefault();
}

LowEngine::Scene& LowEngine::SceneManager::CreateScene(const std::string& name) {
    for (uint32_t i = 0; i < _scenes.size(); i++) {
        if (_scenes[i].Active == false) {
            _scenes[i].Name = name;
            _scenes[i].Active = true;
            return _scenes[i];
        }
    }
    return _scenes[0]; // return default scene
}
