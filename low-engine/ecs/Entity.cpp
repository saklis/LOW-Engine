#include "Entity.h"

// initialize default value to static field
uint32_t Entity::_nextId = 0;

Entity::Entity() {
    this->Id = _nextId++; // assign current value to this instance and then increment
}

void Entity::ActivateAsDefault() {
    this->Name = "Default";
    this->Active = true;
}

void Entity::Activate(const std::string& name) {
    this->Name = name + "_" + std::to_string(this->Id);
    this->Active = true;
}
