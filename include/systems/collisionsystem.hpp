#ifndef MOONLANDER_COLLISIONSYSTEM_HPP
#define MOONLANDER_COLLISIONSYSTEM_HPP

#include "components/collisioncomponent.hpp"
#include "components/spritecomponent.hpp"
#include "components/levelcomponent.hpp"
#include "components/positioncomponent.hpp"
#include "ecs/system.hpp"

using glm::vec2;

class CollisionSystem : public ecs::System<CollisionComponent, SpriteComponent,
        LevelComponent, PositionComponent>
{
    void update_state(size_t delta) override;

private:
    bool levelSpriteCollision(const Sprite &sprite,
                              GLfloat ship_x,
                              GLfloat ship_y, const std::vector<vec2>& points,
                              const std::vector<vec2>& stars, GLfloat angle);
};

#endif //MOONLANDER_COLLISIONSYSTEM_HPP
