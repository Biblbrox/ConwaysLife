#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <memory>

//const GLfloat initYaw = -90.f;
//const GLfloat initPitch = 0.f;
const GLfloat initSens = 0.5f;
const GLfloat initZoom = 45.f;

class Camera
{
protected:
    static std::shared_ptr<Camera> instance;
public:
    Camera(const glm::vec3& pos = glm::vec3(10.f, 10.f, 10.f),
           const glm::vec3& target = glm::vec3(0.f, 0.f, 0.f),
           const glm::vec3& up = glm::vec3(0.f, 1.f, 0.f));

    static std::shared_ptr<Camera> getInstance()
    {
        if (!instance)
            instance = std::make_shared<Camera>();

        return instance;
    }

    GLfloat getX() const;
    GLfloat getY() const;
    GLfloat getZ() const;

    glm::mat4 getView();
    glm::mat4 getProjection(GLfloat screen_width, GLfloat screen_height) const;

    glm::vec3 getPos() const;
    glm::vec3 getTarget() const;
    glm::vec3 getUp() const;

    glm::vec3 getViewDir() const;
    glm::vec3 getRightVec() const;

    void setX(GLfloat x);
    void setY(GLfloat y);
    void setZ(GLfloat z);
    void setPos(const glm::vec3& pos);
    void setView(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up);

    void processMovement(GLfloat xoffset, GLfloat yoffset);
    void processScroll(GLfloat yoffset);

    void updateView();

private:
    glm::vec3 m_pos;
    glm::vec3 m_target;
    glm::vec3 m_up;

    glm::mat4 m_view;

    GLfloat m_mouseSens;
    GLfloat m_zoom;
//    GLfloat m_yaw;
//    GLfloat m_pitch;

    GLfloat m_xOffset;
    GLfloat m_yOffset;
};

#endif //CAMERA_HPP