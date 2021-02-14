#include <memory>
#include "ces/cesmanager.hpp"

std::shared_ptr<Entity> CesManager::createEntity(size_t name)
{
    std::shared_ptr ent = std::make_shared<Entity>();
//    ent->setCesManager(this);
    m_entities.emplace(name, ent);
    return m_entities[name];
}

std::unordered_map<size_t, std::shared_ptr<Entity>>& CesManager::getEntities()
{
    return m_entities;
}