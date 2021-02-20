#ifndef MOONLANDER_POSITIONCOMPONENT_HPP
#define MOONLANDER_POSITIONCOMPONENT_HPP

#include <GL/glew.h>

#include "ecs/component.hpp"

struct PositionComponent : ecs::Component
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
};

#endif //MOONLANDER_POSITIONCOMPONENT_HPP
