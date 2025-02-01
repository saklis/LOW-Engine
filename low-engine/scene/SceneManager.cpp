
#include "SceneManager.h"

void LowEngine::Scene::SceneManager::CreateScene(const std::string& name) {
    _scenes.push(Scene(name));
}

LowEngine::Scene::Scene& LowEngine::Scene::SceneManager::GetCurrent() {
    return _scenes.top();
}
