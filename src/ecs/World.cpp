#include "ecs/World.h"


EntityID World::createEntity() {
    EntityID entityID = EntityIDAllocator::allocate();
    m_entities.insert(entityID);
    return entityID;
}

void World::destroyEntity(EntityID entityID) {
    if (m_entities.find(entityID) == m_entities.end()) {
        return;
    }

    auto entityIt = m_components.find(entityID);
    if (entityIt != m_components.end()) {
        m_components.erase(entityIt);
    }

    m_entityMasks.erase(entityID);

    m_entities.erase(entityID);

    EntityIDAllocator::deallocate(entityID);
}

bool World::isValidEntity(EntityID entityID) const {
    return m_entities.find(entityID) != m_entities.end();
}

void World::update(float deltaTime) {
    for (auto& system : m_systems) {
        if (system->isEnabled()) {
            system->update(deltaTime, shared_from_this());
        }
    }
}

void World::init() {

    for (auto& system : m_systems) {
        system->onAddedToWorld(shared_from_this());
    }
}

void World::shutdown() {

    for (auto& system : m_systems) {
        system->onRemovedFromWorld(shared_from_this());
    }
    m_components.clear();
    m_entityMasks.clear();
    m_entities.clear();
    m_systems.clear();
    m_systemMap.clear();
}