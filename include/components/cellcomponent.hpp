#ifndef CELLCOMPONENT_HPP
#define CELLCOMPONENT_HPP

#include "ecs/component.hpp"

struct CellComponent: ecs::Component
{
    bool alive = false;
    glm::vec4 color;
};

#endif //CELLCOMPONENT_HPP
