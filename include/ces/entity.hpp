#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <memory>
#include <unordered_map>

#include "utils/utils.hpp"
#include "utils/typelist.hpp"

class CesManager;
class Component;

using utils::type_id;
using utils::TypeList;
using utils::IsBaseOfRec;
using utils::Length;

/**
 * Entity class
 * Each entity may contain several unique components
 */
class Entity
{
public:

    Entity() : m_alive(false) {}
    ~Entity() {};
    Entity(Entity&& en) = default;
    Entity(Entity& en) = default;
    Entity& operator=(Entity&&) = default;
    Entity& operator=(const Entity&) = default;

    /**
     * Create new component and return
     * ComponentType must be child of Component class
     * @tparam ComponentType
     * @return
     */
    template <class ComponentType>
    std::shared_ptr<Component> addComponent()
    {
        static_assert(std::is_base_of_v<Component, ComponentType>,
                      "Template parameter class must be child of Component");
        m_components[type_id<ComponentType>()] =
                std::static_pointer_cast<Component>(std::make_shared<ComponentType>());
        return m_components[type_id<ComponentType>()];
    }

    template <class ...ComponentTypes>
    void addComponents()
    {
        static_assert(IsBaseOfRec<Component, TypeList<ComponentTypes...>>::value,
                      "Template parameter class must be child of Component");
        using ComponentList = utils::TypeList<ComponentTypes...>;
        static_assert(utils::Length<ComponentList>::value >= 2,
                      "Length of ComponentTypes must be greeter than 2");

        auto bin = [](auto x, auto y){ return 0; };

        auto un = [this](auto x){
            m_components[type_id<decltype(x)>()] =
                    std::static_pointer_cast<Component>(std::make_shared<decltype(x)>());
            return 0;
        };

        utils::typeListReduce<ComponentList>(un, bin);
    }


    /**
     * Get component by type
     * @tparam ComponentType
     * @return
     */
    template <class ComponentType>
    std::unordered_map<size_t, std::shared_ptr<Component>>::iterator
    getComponentIt()
    {
        static_assert(std::is_base_of_v<Component, ComponentType>,
                      "Template parameter class must be child of Component");
        auto it = m_components.find(type_id<ComponentType>());
        return it;
    }

    /**
     * Get component by type
     * @tparam ComponentType
     * @return
     */
    template <class ComponentType>
    std::shared_ptr<ComponentType> getComponent()
    {
        static_assert(std::is_base_of_v<Component, ComponentType>,
                      "Template parameter class must be child of Component");

        auto it = m_components.find(type_id<ComponentType>());
        if (it == m_components.end())
            return std::shared_ptr<ComponentType>(nullptr);

        return std::dynamic_pointer_cast<ComponentType>(it->second);
    }

    template <class ComponentType>
    void removeComponent()
    {
        static_assert(std::is_base_of_v<Component, ComponentType>,
                      "Template parameter class must be child of Component");

        auto it = m_components.find(type_id<ComponentType>());
        if (it != m_components.end())
            m_components.erase(it);
    }

    const std::unordered_map<size_t, std::shared_ptr<Component>>&
    getComponents() const;

    void setCesManager(CesManager* cesMan);

    void activate();
    bool isActivate() const;
    void kill();

private:
    std::unordered_map<size_t, std::shared_ptr<Component>> m_components;
    CesManager* m_cesManager;
    bool m_alive;
};

#endif //ENTITY_HPP
