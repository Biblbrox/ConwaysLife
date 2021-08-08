#ifndef RENDERERSYSTEM_HPP
#define RENDERERSYSTEM_HPP

#include <GL/glew.h>

#include "render/camera.hpp"
#include "ecs/system.hpp"
#include "components/positioncomponent.hpp"

/**
 * System that can handle level surface
 */
class RendererSystem : public ecs::System<PositionComponent>
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

    GLuint m_instanceVBO;

    GLfloat m_aspectRatio;

    bool m_videoSettingsOpen;
    bool m_colorSettingsOpen;
    size_t m_fieldSize;

    bool m_isMsaa;
    bool m_coloredGame;

    void drawGui();
};

#endif //RENDERERSYSTEM_HPP
