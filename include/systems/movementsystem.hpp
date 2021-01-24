#ifndef MOONLANDER_MOVEMENTSYSTEM_HPP
#define MOONLANDER_MOVEMENTSYSTEM_HPP

#include <GL/glew.h>

#include "components/velocitycomponent.hpp"
#include "components/particlespritecomponent.hpp"
#include "ces/system.hpp"
#include "components/positioncomponent.hpp"

/**
 * System that can handle game objects with
 * Position and Direction components, like spaceship
 */
class MovementSystem: public
        System<PositionComponent, VelocityComponent, ParticleSpriteComponent>
{
public:
    explicit MovementSystem();

    void update_state(size_t delta) override;
};

#endif //MOONLANDER_MOVEMENTSYSTEM_HPP
