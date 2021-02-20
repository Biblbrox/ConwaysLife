#ifndef MOONLANDER_TEXTCOMPONENT_HPP
#define MOONLANDER_TEXTCOMPONENT_HPP

#include <memory>

#include "render/texttexture.hpp"
#include "ecs/component.hpp"

struct TextComponent : ecs::Component
{
    std::shared_ptr<TextTexture> texture;
};

#endif //MOONLANDER_TEXTCOMPONENT_HPP
