#ifndef RENDERERSYSTEM_HPP
#define RENDERERSYSTEM_HPP

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
    void drawLevel();
    void drawText();
    void drawToFramebuffer();

    GLuint m_frameBuffer;
    GLuint m_frameBufTex;
    GLfloat m_aspectRatio;

    bool m_videoSettingsOpen;

    void drawGui();
};

#endif //RENDERERSYSTEM_HPP
