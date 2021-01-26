#ifndef MOONLANDER_POSITIONCOMPONENT_HPP
#define MOONLANDER_POSITIONCOMPONENT_HPP

#include <GL/glew.h>

#include "ces/component.hpp"

struct PositionComponent : Component
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat angle;
    GLfloat scale_factor = 1.f;
    GLfloat scallable = true;
};

#endif //MOONLANDER_POSITIONCOMPONENT_HPP
