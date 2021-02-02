#ifndef MOONLANDER_KEYBOARDSYSTEM_HPP
#define MOONLANDER_KEYBOARDSYSTEM_HPP

#include "components/keyboardcomponent.hpp"
#include "ces/system.hpp"

class KeyboardSystem : public System<KeyboardComponent>
{
public:
    explicit KeyboardSystem();
private:
    void update_state(size_t delta) override;

    bool m_middlePressed;
};

#endif //MOONLANDER_KEYBOARDSYSTEM_HPP
