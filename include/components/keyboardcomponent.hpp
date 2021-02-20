#ifndef MOONLANDER_KEYBOARDCOMPONENT_HPP
#define MOONLANDER_KEYBOARDCOMPONENT_HPP

#include <functional>
#include <SDL_quit.h>

#include "ecs/component.hpp"

struct KeyboardComponent : ecs::Component
{
    std::function<void(const Uint8*)> event_handler;
};

#endif //MOONLANDER_KEYBOARDCOMPONENT_HPP
