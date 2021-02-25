#ifndef CELLCOMPONENT_HPP
#define CELLCOMPONENT_HPP

#include "ecs/component.hpp"
#include "render/sprite.hpp"

//struct CellComponent: ecs::Component
struct CellComponent
{
    bool alive = false;
//    char neirCount;
    glm::vec3 color = {0.f, 0.f, 0.f};
    glm::vec3 pos;
    static std::shared_ptr<Sprite> sprite;
};

#endif //CELLCOMPONENT_HPP
