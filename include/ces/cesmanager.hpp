#ifndef CESMANAGER_HPP
#define CESMANAGER_HPP

#include <string>

#include "ces/entity.hpp"
#include "ces/basesystem.hpp"

class CesManager
{
public:
    virtual void init() = 0;
    virtual void update(size_t delta) = 0;

    virtual std::shared_ptr<Entity> createEntity(size_t name);

    template <typename SystemType>
    SystemType& createSystem()
    {
        static_assert(std::is_base_of_v<BaseSystem, SystemType>,
                      "Template parameter class must be child of BaseSystem");

        std::shared_ptr<SystemType> system(new SystemType());
        system->setCesManager(this);
        m_systems.insert({type_id<SystemType>(),
                          std::static_pointer_cast<BaseSystem>(system)});
        return *system;
    }

    virtual std::unordered_map<size_t, std::shared_ptr<Entity>>& getEntities();

protected:
    std::unordered_map<size_t, std::shared_ptr<Entity>> m_entities;
    std::unordered_map<size_t, std::shared_ptr<BaseSystem>> m_systems;
};

#endif //CESMANAGER_HPP
