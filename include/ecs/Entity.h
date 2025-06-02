#pragma once
#include "pch.h"

using EntityID = std::uint32_t;

inline constexpr EntityID NULL_ENTITY_ID = 0;

class EntityIDAllocator {
public:
    static EntityID allocate() {
        {
            std::lock_guard<std::mutex> lock(s_mutex);
            if (!s_freedIDs.empty()) {
                EntityID id = s_freedIDs.back();
                s_freedIDs.pop_back();
                return id;
            }
        }
        return s_nextID.fetch_add(1, std::memory_order_relaxed);
    }

    static void deallocate(EntityID id) {
        if (id != NULL_ENTITY_ID) {
            std::lock_guard<std::mutex> lock(s_mutex);
            s_freedIDs.push_back(id);
        }
    }
private:
    static inline std::atomic<EntityID> s_nextID{ 1 };
    static inline std::vector<EntityID> s_freedIDs;
    static inline std::mutex s_mutex;

};

class EntityManager {
public:
    EntityID createEntity();
    void DestroyEntity(EntityID entityID);
    bool isValidEntity(EntityID entityID) const;
    const std::unordered_set<EntityID>& getAllEntities() const { return m_entities; }

private:
    std::unordered_set<EntityID> m_entities;
};
