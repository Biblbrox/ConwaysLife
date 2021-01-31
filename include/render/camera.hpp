#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

const GLfloat initYaw = -90.f;
const GLfloat initPitch = 0.f;
const GLfloat initSpeed = 1.f;
const GLfloat initSens = 0.1f;
const GLfloat initZoom = 45.f;

enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera
{
public:
    Camera(const glm::vec3& pos = glm::vec3(0.f, 0.f, 0.f),
           const glm::vec3& up = glm::vec3(0.f, 1.f, 0.f),
           GLfloat yaw = initYaw, GLfloat pitch = initPitch);

    glm::mat4 getView() const;
    glm::mat4 getProjection(GLfloat screen_width, GLfloat screen_height) const;

    GLfloat getX() const;
    GLfloat getY() const;
    GLfloat getZ() const;
    glm::vec3 getPos() const;

    void setX(GLfloat x);
    void setY(GLfloat y);
    void setZ(GLfloat z);
    void setPos(const glm::vec3& pos);

    void processMovement(GLfloat xoffset, GLfloat yoffset, GLfloat constrainPitch = true);
    void processScroll(GLfloat yoffset);

private:
    glm::vec3 m_pos;

    GLfloat m_movSpeed;
    GLfloat m_mouseSens;
    GLfloat m_zoom;
    GLfloat m_yaw;
    GLfloat m_pitch;
};

#endif //CAMERA_HPP