#ifndef MOONLANDER_PHYSICSSYSTEM_HPP
#define MOONLANDER_PHYSICSSYSTEM_HPP

#include "components/particlespritecomponent.hpp"
#include "ces/system.hpp"

class PhysicsSystem : public System <ParticleSpriteComponent>
{
public:
    void update_state(size_t delta) override;
};

#endif //MOONLANDER_PHYSICSSYSTEM_HPP
