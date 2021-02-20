#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <vector>
#include <set>
#include <memory>

#include "typelist.hpp"
#include "basesystem.hpp"
#include "entity.hpp"
#include "ecsmanager.hpp"

namespace ecs
{
    /**
     * Specialization
     */
    template<typename ...Args>
    class System : public BaseSystem
    {
    public:
        explicit System()
        {
            m_componentTypes.insert({types::type_id<Args>...});
        }

        virtual ~System() = default;

        /**
         * Returns entities which corresponds to the componentTypes container filter
         * @return
         */
        auto getEntities() const
        {
            auto filtered = m_ecsManager->getEntities();
            for (auto it = filtered.begin(); it != filtered.end();) {
                auto components = it->second->getComponents();
                if (std::any_of(m_componentTypes.begin(), m_componentTypes.end(),
                                [&components](size_t t) {
                                    return components.find(t) == components.end();
                                }))
                    it = filtered.erase(it);
                else
                    ++it;
            }

            return filtered;
        }

        /**
         * Return entities by specific tags
         * @tparam ComponentTypes
         * @return
         */
        template<typename... ComponentTypes>
        auto getEntitiesByTags() const
        {
            static_assert(types::IsBaseOfRec<Component, types::TypeList<ComponentTypes...>>::value,
                          "Template parameter class must be child of Component");
            using ComponentList = types::TypeList<ComponentTypes...>;
            static_assert(types::Length<ComponentList>::value >= 2,
                          "Length of ComponentTypes must be greeter than 2");

            auto bin = [](bool x, bool y) { return x && y; };

            auto filtered = m_ecsManager->getEntities();
            for (auto it = filtered.begin(); it != filtered.end();) {
                // Lambda to check that current entity (it) has
                // each of ComponentTypes
                auto un = [it](auto x) {
                    return it->second->getComponent<decltype(x)>() != nullptr;
                };
                if (!typeListReduce<ComponentList>(un, bin))
                    it = filtered.erase(it);
                else
                    ++it;
            }

            return filtered;
        }

        /**
         * Return entities by specific tag
         * @tparam ComponentType
         * @return
         */
        template<class ComponentType>
        auto getEntitiesByTag() const
        {
            static_assert(std::is_base_of_v<Component, ComponentType>,
                          "ComponentType class must be child of Component");
            auto &filtered = m_ecsManager->getEntities();
            for (auto it = filtered.begin(); it != filtered.end();) {
                if (!it->second->getComponent<ComponentType>())
                    it = filtered.erase(it);
                else
                    ++it;
            }

            return filtered;
        }

    private:
        // Contains id's of each component type system can handle
        std::set<size_t> m_componentTypes;
    };
};

#endif //SYSTEM_HPP
