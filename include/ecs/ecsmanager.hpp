#ifndef ECSMANAGER_HPP
#define ECSMANAGER_HPP

#include <string>

#include "entity.hpp"
#include "basesystem.hpp"

namespace ecs
{
    /**
     * ECS manager class.
     * It must create systems and update them
     * This also used by systems to fetch entities
     */
    class EcsManager
    {
    public:
        /**
         * Create systems, etc...
         */
        virtual void init() = 0;

        /**
         * Update systems
         * @param delta
         */
        virtual void update(size_t delta) = 0;

        virtual std::shared_ptr<Entity> createEntity(size_t name)
        {
            std::shared_ptr ent = std::make_shared<Entity>();
            m_entities.emplace(name, ent);
            return m_entities[name];
        }

        template<typename SystemType>
        SystemType &createSystem()
        {
            static_assert(std::is_base_of_v<BaseSystem, SystemType>,
                          "Template parameter class must be child of BaseSystem");

            std::shared_ptr<SystemType> system(new SystemType());
            system->setEcsManager(this);
            m_systems.insert({types::type_id<SystemType>,
                              std::static_pointer_cast<BaseSystem>(system)});
            return *system;
        }

        virtual std::unordered_map<size_t, std::shared_ptr<Entity>> &getEntities()
        {
            return m_entities;
        }

    protected:
        std::unordered_map<size_t, std::shared_ptr<Entity>> m_entities;
        std::unordered_map<size_t, std::shared_ptr<BaseSystem>> m_systems;
    };
};

#endif //ECSMANAGER_HPP
