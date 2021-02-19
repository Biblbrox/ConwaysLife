#ifndef CELLCOMPONENT_HPP
#define CELLCOMPONENT_HPP

#include "ces/component.hpp"

struct CellComponent: Component
{
    bool alive = false;
    glm::vec4 color;
};

#endif //CELLCOMPONENT_HPP
