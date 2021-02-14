#include <memory>
#include "ces/component.hpp"
#include "ces/entity.hpp"

void Entity::activate()
{
    m_alive = true;
}

const std::unordered_map<size_t, std::shared_ptr<Component>>&
Entity::getComponents() const
{
    return m_components;
}

//void Entity::setCesManager(CesManager* cesMan)
//{
//    m_cesManager = cesMan;
//}

bool Entity::isActivate() const
{
    return m_alive;
}

void Entity::kill()
{
    m_alive = false;
}
