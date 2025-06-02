#pragma once
#include "pch.h"
#include "ecs/Component.h"
#include "ecs/Entity.h"
class World;

class ISystem {
public:
    virtual ~ISystem() = default;

    virtual void update(float deltaTime, std::shared_ptr<World> world) = 0;

    virtual void onAddedToWorld(std::shared_ptr<World> world) {}

    virtual void onRemovedFromWorld(std::shared_ptr<World> world) {}

    virtual bool isEnabled() const { return m_enabled; }
    virtual void setEnabled(bool enabled) { m_enabled = enabled; }

    virtual std::string getName() const { return m_name; }
    virtual void setName(const std::string& name) { m_name = name; }

protected:
    bool m_enabled = true;
    std::string m_name;
};