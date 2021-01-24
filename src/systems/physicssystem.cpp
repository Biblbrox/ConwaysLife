#include "systems/physicssystem.hpp"

void PhysicsSystem::update_state(size_t delta)
{
    auto entities = getEntitiesByTag<VelocityComponent>();
    for (auto& [key, en] : entities)
        en->getComponent<VelocityComponent>()->y += gravity_force / weight;

    auto particles = getEntitiesByTag<ParticleSpriteComponent>();
    for (auto& [key, en] : particles) {
        auto& vels = en->getComponent<ParticleSpriteComponent>()->vel;
        for (auto& vel: vels)
            vel.y += gravity_force / weight;
    }
}
