#ifndef MOONLANDER_PHYSICSSYSTEM_HPP
#define MOONLANDER_PHYSICSSYSTEM_HPP

#include "ecs/system.hpp"

class PhysicsSystem : public ecs::System <>
{
public:
    void update_state(size_t delta) override;
};

#endif //MOONLANDER_PHYSICSSYSTEM_HPP
