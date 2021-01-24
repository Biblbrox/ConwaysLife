#ifndef MOONLANDER_TEXTCOMPONENT_HPP
#define MOONLANDER_TEXTCOMPONENT_HPP

#include <memory>

#include "render/texttexture.hpp"
#include "ces/component.hpp"

struct TextComponent : Component
{
    std::shared_ptr<TextTexture> texture;
};

#endif //MOONLANDER_TEXTCOMPONENT_HPP
