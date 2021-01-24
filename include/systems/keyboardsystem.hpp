#ifndef MOONLANDER_KEYBOARDSYSTEM_HPP
#define MOONLANDER_KEYBOARDSYSTEM_HPP

#include "components/keyboardcomponent.hpp"
#include "ces/system.hpp"

class KeyboardSystem : public System<KeyboardComponent>
{
    void update_state(size_t delta) override;
};

#endif //MOONLANDER_KEYBOARDSYSTEM_HPP
