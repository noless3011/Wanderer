#pragma once
#include "pch.h"
#include "ecs/ComponentMask.h"


using ComponentID = std::uint32_t;

inline constexpr ComponentID NULL_COMPONENT_TYPE = 0;

struct IComponent {
    virtual ~IComponent() = default;
};

class ComponentTypeIDAllocator {
public:
    static ComponentID allocate() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_freedIDs.empty()) {
            ComponentID id = m_freedIDs.back();
            m_freedIDs.pop_back();
            return id;
        }
        return m_nextID.fetch_add(1, std::memory_order_relaxed);
    }

    static void deallocate(ComponentID id) {
        if (id != NULL_COMPONENT_TYPE) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_freedIDs.push_back(id);
        }
    }
private:
    static inline std::atomic<ComponentID> m_nextID{ 1 };
    static inline std::vector<ComponentID> m_freedIDs;
    static inline std::mutex m_mutex;
};

class ComponentManager {
public:
    template<typename T, typename... Args>
    T& addComponent(EntityID entityID, Args&&... args);

    template<typename T>
    T& getComponent(EntityID entityID);

    template<typename T>
    void removeComponent(EntityID entityID);

    template<typename T>
    bool hasComponent(EntityID entityID) const;

private:
    std::unordered_map<EntityID, std::unordered_map<ComponentID, std::unique_ptr<IComponent>>> m_components;

    std::unordered_map<EntityID, ComponentMask> m_entityMasks;
};