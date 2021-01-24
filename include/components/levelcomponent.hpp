#ifndef MOONLANDER_LEVELCOMPONENT_HPP
#define MOONLANDER_LEVELCOMPONENT_HPP

#include "ces/component.hpp"

using glm::vec2;

struct LevelComponent : Component
{
    std::vector<vec2> points;
    std::vector<vec2> stars;
    std::vector<vec2> platforms;

    GLfloat scale_factor = 1.f;
};

#endif //MOONLANDER_LEVELCOMPONENT_HPP
