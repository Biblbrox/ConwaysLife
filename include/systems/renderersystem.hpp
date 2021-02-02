#ifndef MOONLANDER_RENDERERSYSTEM_HPP
#define MOONLANDER_RENDERERSYSTEM_HPP

#include <GL/glew.h>

#include "render/camera.hpp"
#include "components/textcomponent.hpp"
#include "ces/system.hpp"
#include "components/positioncomponent.hpp"

/**
 * System that can handle level surface
 */
class RendererSystem : public System<PositionComponent, TextComponent>
{
public:
    explicit RendererSystem();
    ~RendererSystem();

    void update_state(size_t delta) override;
private:
    void drawSprites();
    void drawGui();
    void drawLevel();
    void drawText();

    GLuint m_frameBuffer;
    GLuint m_frameBufTex;
};

#endif //MOONLANDER_RENDERERSYSTEM_HPP
