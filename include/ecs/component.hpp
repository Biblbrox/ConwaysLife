#ifndef COMPONENT_HPP
#define COMPONENT_HPP

namespace ecs
{
    /**
     * Base class for component
     */
    class Component
    {
    public:
        Component(){};

        virtual ~Component() = default;
    };
};

#endif //COMPONENT_HPP
