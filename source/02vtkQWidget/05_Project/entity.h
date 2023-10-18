#pragma once

#include <atomic>

using Entity = uint32_t;

class EntityManager
{

public:
    static EntityManager* Instance()
    {
        static EntityManager m;
        return &m;
    }

    Entity Create()
    {
        return m_id++;
    }

private:
    EntityManager() noexcept                       = default;
    ~EntityManager() noexcept                      = default;
    EntityManager(const EntityManager&)            = delete;
    EntityManager& operator=(const EntityManager&) = delete;

private:
    std::atomic_uint32_t m_id;
};