#ifndef RENDERERSYSTEM_HPP
#define RENDERERSYSTEM_HPP

#include <GL/glew.h>

#include "render/camera.hpp"
#include "components/textcomponent.hpp"
#include "ecs/system.hpp"
#include "components/positioncomponent.hpp"

/**
 * System that can handle level surface
 */
class RendererSystem : public ecs::System<PositionComponent, TextComponent>
{
public:
    explicit RendererSystem();
    ~RendererSystem();

    void update_state(size_t delta) override;
private:
    void drawSprites();
    void drawToFramebuffer();

    GLuint m_frameBufferMSAA;
    GLuint m_frameBuffer;

    GLuint m_frameBufTex;
    GLuint m_frameBufTexMSAA;

    GLfloat m_aspectRatio;

    bool m_videoSettingsOpen;
    bool m_colorSettingsOpen;

    bool m_isMsaa;

    void drawGui();
};

#endif //RENDERERSYSTEM_HPP
