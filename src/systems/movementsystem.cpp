#include "systems/movementsystem.hpp"

void MovementSystem::update_state(size_t delta)
{
    auto entities = getEntitiesByTags<PositionComponent, VelocityComponent>();
    for (auto& [key, en]: entities) {
        auto pos = en->getComponent<PositionComponent>();
        const auto vel = en->getComponent<VelocityComponent>();

        pos->x += vel->x;
        pos->y += vel->y;
        pos->angle += vel->angle;
    }

    auto particles = getEntitiesByTag<ParticleSpriteComponent>();
    for (auto& [key, en]: particles) {
        auto particle =  en->getComponent<ParticleSpriteComponent>();

        auto& coords = particle->coords;
        const auto& vel = particle->vel;
        assert(coords.size() == vel.size()
               && "Number of coordinates must be "
                  "the same as number of velocities");
        for(size_t i = 0; i < coords.size(); ++i) {
            coords[i].x += vel[i].x;
            coords[i].y += vel[i].y;
            coords[i].angle += vel[i].angle;
        }
    }
}

MovementSystem::MovementSystem()
{

}
