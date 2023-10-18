#pragma once

#include "entity.h"
#include "iviewer.h"
#include <map>

class ActorManager
{
public:
    void AddActor(Entity e, ActorData* actor)
    {
        m_actors[e] = actor;
    }

    ActorData* GetActor(Entity e) const
    {
        return m_actors.at(e);
    }

    uint32_t GetId(Entity e) const
    {
        return m_actors.at(e)->id;
    }

private:
    std::map<Entity, ActorData*> m_actors;
};