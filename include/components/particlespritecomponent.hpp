#ifndef MOONLANDER_PARTICLESPRITECOMPONENT_HPP
#define MOONLANDER_PARTICLESPRITECOMPONENT_HPP

#include <GL/glew.h>

#include "ces/component.hpp"
#include "render/sprite.hpp"

/**
 * Particle Sprite Component. Each particle is clip of one sprite.
 */
struct ParticleSpriteComponent : Component
{
    bool is_alive = false;
    GLfloat life_time;

    std::shared_ptr<Sprite> sprite;

    /**
     * Vector that contains coordinate of each particle
     * in tuple <x, y, angle>
     */
    std::vector<utils::Position> coords;

    /**
     * Vector that contains velocity of each particle
     * in tuple <vel_x, vel_y, vel_angle>
     */
    std::vector<utils::Position> vel;
};

#endif //MOONLANDER_PARTICLESPRITECOMPONENT_HPP
