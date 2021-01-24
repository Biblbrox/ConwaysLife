#ifndef MOONLANDER_SPRITECOMPONENT_HPP
#define MOONLANDER_SPRITECOMPONENT_HPP

#include "render/sprite.hpp"
#include "ces/component.hpp"

struct SpriteComponent : Component
{
    std::shared_ptr<Sprite> sprite;
};

#endif //MOONLANDER_SPRITECOMPONENT_HPP
