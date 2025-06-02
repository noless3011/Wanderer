#pragma once
#include "pch.h"
#include "ecs/Entity.h"
#include "ecs/Component.h"
#include "ecs/System.h"
#include "ecs/ComponentMask.h"

class World : public std::enable_shared_from_this<World> {
public:
    World() = default;
    ~World() = default;


    EntityID createEntity();
    void destroyEntity(EntityID entityID);
    bool isValidEntity(EntityID entityID) const;


    template<typename T, typename... Args>
    T& addComponent(EntityID entityID, Args&&... args);

    template<typename T>
    T* getComponent(EntityID entityID);

    template<typename T>
    const T* getComponent(EntityID entityID) const;

    template<typename T>
    void removeComponent(EntityID entityID);

    template<typename T>
    bool hasComponent(EntityID entityID) const;


    template<typename T, typename... Args>
    std::shared_ptr<T> addSystem(Args&&... args);

    template<typename T>
    std::shared_ptr<T> getSystem();

    template<typename T>
    void removeSystem();

    template<typename... Components>
    std::vector<EntityID> getEntitiesWith() const;    template<typename... Components, typename Func>
        void forEach(Func&& func);

    template<typename... Components, typename Func>
    void forEach(Func&& func) const;

    const std::unordered_set<EntityID>& getAllEntities() const { return m_entities; }

    void update(float deltaTime);

    void init();
    void shutdown();

private:
    std::unordered_set<EntityID> m_entities;

    std::unordered_map<EntityID, std::unordered_map<ComponentID, std::unique_ptr<IComponent>>> m_components;

    std::unordered_map<EntityID, ComponentMask> m_entityMasks;

    std::vector<std::shared_ptr<ISystem>> m_systems;
    std::unordered_map<std::type_index, std::shared_ptr<ISystem>> m_systemMap;    template<typename T>
        static ComponentID getComponentTypeID();

    static inline std::unordered_map<std::type_index, ComponentID> s_componentTypeIDs;
    static inline std::mutex s_componentTypeMutex;

    friend class ComponentMask;
};

template<typename T>
ComponentID World::getComponentTypeID() {
    std::lock_guard<std::mutex> lock(s_componentTypeMutex);
    std::type_index typeIndex = std::type_index(typeid(T));

    auto it = s_componentTypeIDs.find(typeIndex);
    if (it != s_componentTypeIDs.end()) {
        return it->second;
    }

    ComponentID newID = ComponentTypeIDAllocator::allocate();
    s_componentTypeIDs[typeIndex] = newID;
    return newID;
}

template<typename T, typename... Args>
T& World::addComponent(EntityID entityID, Args&&... args) {
    static_assert(std::is_base_of_v<IComponent, T>, "T must inherit from IComponent");

    ComponentID typeID = getComponentTypeID<T>();
    auto component = std::make_unique<T>(std::forward<Args>(args)...);
    T& ref = *component;

    m_components[entityID][typeID] = std::move(component);


    m_entityMasks[entityID].set<T>();

    return ref;
}

template<typename T>
T* World::getComponent(EntityID entityID) {
    ComponentID typeID = getComponentTypeID<T>();

    auto entityIt = m_components.find(entityID);
    if (entityIt == m_components.end()) {
        return nullptr;
    }

    auto componentIt = entityIt->second.find(typeID);
    if (componentIt == entityIt->second.end()) {
        return nullptr;
    }

    return static_cast<T*>(componentIt->second.get());
}

template<typename T>
const T* World::getComponent(EntityID entityID) const {
    ComponentID typeID = getComponentTypeID<T>();

    auto entityIt = m_components.find(entityID);
    if (entityIt == m_components.end()) {
        return nullptr;
    }

    auto componentIt = entityIt->second.find(typeID);
    if (componentIt == entityIt->second.end()) {
        return nullptr;
    }

    return static_cast<const T*>(componentIt->second.get());
}

template<typename T>
void World::removeComponent(EntityID entityID) {
    ComponentID typeID = getComponentTypeID<T>();

    auto entityIt = m_components.find(entityID);
    if (entityIt != m_components.end()) {
        entityIt->second.erase(typeID);
        if (entityIt->second.empty()) {
            m_components.erase(entityIt);
        }
    }


    auto maskIt = m_entityMasks.find(entityID);
    if (maskIt != m_entityMasks.end()) {
        maskIt->second.unset<T>();
        if (maskIt->second.empty()) {
            m_entityMasks.erase(maskIt);
        }
    }
}

template<typename T>
bool World::hasComponent(EntityID entityID) const {
    ComponentID typeID = getComponentTypeID<T>();

    auto entityIt = m_components.find(entityID);
    if (entityIt == m_components.end()) {
        return false;
    }

    return entityIt->second.find(typeID) != entityIt->second.end();
}

template<typename T, typename... Args>
std::shared_ptr<T> World::addSystem(Args&&... args) {
    static_assert(std::is_base_of_v<ISystem, T>, "T must inherit from ISystem");

    auto system = std::make_shared<T>(std::forward<Args>(args)...);
    m_systems.push_back(system);
    m_systemMap[std::type_index(typeid(T))] = system;

    system->onAddedToWorld(shared_from_this());
    return system;
}

template<typename T>
std::shared_ptr<T> World::getSystem() {
    auto it = m_systemMap.find(std::type_index(typeid(T)));
    if (it != m_systemMap.end()) {
        return std::static_pointer_cast<T>(it->second);
    }
    return nullptr;
}

template<typename T>
void World::removeSystem() {
    std::type_index typeIndex = std::type_index(typeid(T));
    auto it = m_systemMap.find(typeIndex);

    if (it != m_systemMap.end()) {
        auto system = it->second;
        system->onRemovedFromWorld(shared_from_this());

        m_systems.erase(std::remove(m_systems.begin(), m_systems.end(), system), m_systems.end());
        m_systemMap.erase(it);
    }
}


template<typename... Components>
std::vector<EntityID> World::getEntitiesWith() const {
    std::vector<EntityID> result;

    for (const auto& entityID : m_entities) {
        if ((hasComponent<Components>(entityID) && ...)) {
            result.push_back(entityID);
        }
    }

    return result;
}

template<typename... Components, typename Func>
void World::forEach(Func&& func) {
    for (const auto& entityID : m_entities) {
        if ((hasComponent<Components>(entityID) && ...)) {
            func(entityID, *getComponent<Components>(entityID)...);
        }
    }
}

template<typename... Components, typename Func>
void World::forEach(Func&& func) const {
    for (const auto& entityID : m_entities) {
        if ((hasComponent<Components>(entityID) && ...)) {
            func(entityID, *getComponent<Components>(entityID)...);
        }
    }
}


template<typename T>
void ComponentMask::set() {
    static_assert(std::is_base_of_v<IComponent, T>, "T must inherit from IComponent");
    ComponentID id = World::getComponentTypeID<T>();
    if (id < MAX_COMPONENTS) {
        m_mask.set(id);
    }
}

template<typename T>
void ComponentMask::unset() {
    static_assert(std::is_base_of_v<IComponent, T>, "T must inherit from IComponent");
    ComponentID id = World::getComponentTypeID<T>();
    if (id < MAX_COMPONENTS) {
        m_mask.reset(id);
    }
}

template<typename T>
bool ComponentMask::has() const {
    static_assert(std::is_base_of_v<IComponent, T>, "T must inherit from IComponent");
    ComponentID id = World::getComponentTypeID<T>();
    return id < MAX_COMPONENTS && m_mask.test(id);
}

template<typename T>
ComponentID ComponentMask::getComponentTypeID() {
    return World::getComponentTypeID<T>();
}
