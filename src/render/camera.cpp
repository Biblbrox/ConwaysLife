#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <iostream>

#include "render/camera.hpp"

std::shared_ptr<Camera> Camera::instance = nullptr;

Camera::Camera(const glm::vec3& pos, const glm::vec3& up, GLfloat yaw, GLfloat pitch)
        : m_mouseSens(initSens), m_zoom(initZoom)
{
    m_pos = pos;
    m_yaw = yaw;
    m_pitch = pitch;
}

GLfloat Camera::getX() const
{
    return m_pos.x;
}

GLfloat Camera::getY() const
{
    return m_pos.y;
}

GLfloat Camera::getZ() const
{
    return m_pos.z;
}

void Camera::setX(GLfloat x)
{
    m_pos.x = x;
}

void Camera::setY(GLfloat y)
{
    m_pos.y = y;
}

void Camera::setZ(GLfloat z)
{
    m_pos.z = z;
}

glm::mat4 Camera::getView() const
{
    return glm::translate(glm::mat4(1.f), m_pos)
           * glm::eulerAngleXY(glm::radians(m_pitch),
                               glm::radians(m_yaw));
}

void Camera::setPos(const glm::vec3& pos)
{
    m_pos = pos;
}

glm::vec3 Camera::getPos() const
{
    return m_pos;
}

void Camera::processMovement(GLfloat xoffset, GLfloat yoffset,
                             GLfloat constrainPitch)
{
    xoffset *= m_mouseSens;
    yoffset *= m_mouseSens;


    m_yaw += xoffset;
    m_pitch += yoffset;

    if (constrainPitch) {
        if (m_pitch > 89.f)
            m_pitch = 89.f;
        if (m_pitch < -89.f)
            m_pitch = -89.f;
    }
}

void Camera::processScroll(GLfloat yoffset)
{
    m_zoom -= yoffset;
    if (m_zoom < 1.f)
        m_zoom = 1.f;
    if (m_zoom > 90.f)
        m_zoom = 90.f;
}

glm::mat4 Camera::getProjection(GLfloat screen_width, GLfloat screen_height) const
{
    return glm::perspective(
            m_zoom,
            (float)screen_width / (float)screen_height, 0.1f, 1000.f);
}
