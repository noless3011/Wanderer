#pragma once
#include "pch.h"


struct IComponent;
using ComponentID = std::uint32_t;


class ComponentMask {
public:
    static constexpr size_t MAX_COMPONENTS = 64;

    ComponentMask() = default;

    template<typename T>
    void set();

    template<typename T>
    void unset();

    template<typename T>
    bool has() const;

    bool matches(const ComponentMask& other) const {
        return (m_mask & other.m_mask) == other.m_mask;
    }

    void clear() {
        m_mask.reset();
    }

    bool empty() const {
        return m_mask.none();
    }

    const std::bitset<MAX_COMPONENTS>& getBitset() const {
        return m_mask;
    }

private:
    std::bitset<MAX_COMPONENTS> m_mask;

    template<typename T>
    static ComponentID getComponentTypeID();
};
