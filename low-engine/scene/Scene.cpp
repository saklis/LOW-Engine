
#include "Scene.h"

Scene::Scene(std::string name): Name(std::move(name)) {
}

uint32_t Scene::AddEntity(const std::string& name) {
    return _memory.CreateEntity(name);
}

template<typename T, typename ... Args>
uint32_t Scene::AddComponent(uint32_t entityId, Args&&... args) {
    return _memory.CreateComponent<T>(entityId, std::forward<Args>(args)...);
}

template uint32_t Scene::AddComponent<TransformComponent>(uint32_t entityId);