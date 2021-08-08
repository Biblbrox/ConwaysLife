#ifndef CELLCOMPONENT_HPP
#define CELLCOMPONENT_HPP

#include "ecs/component.hpp"
#include "render/sprite.hpp"

struct CellComponent
{
    int8_t alive = 0;
    glm::vec3 color = {0.f, 0.f, 0.f};
    static std::shared_ptr<Sprite> sprite;
};

#endif //CELLCOMPONENT_HPP
