
#include "Scene.h"

LowEngine::Scene::Scene(const std::string& name): Name(name), _memory() {
}

void LowEngine::Scene::InitAsDefault() {
    Active = true;
    Name = "Default scene";
}

void LowEngine::Scene::Update() {
}

uint32_t LowEngine::Scene::AddEntity(const std::string& name) {
    return _memory.CreateEntity(name);
}
