#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "render/camera.hpp"

GLfloat Camera::getX() const
{

}

GLfloat Camera::getY() const
{

}

void Camera::setX(GLfloat x)
{

}

void Camera::setY(GLfloat y)
{

}

Camera::Camera() : m_pos(0.f, 0.f, -3.f),
                   m_front(0.f, 0.f, -1.f),
                   m_up(0.f, 1.f, 0.f)
{
}

glm::mat4 Camera::getView() const
{
    return glm::lookAt(m_pos, m_pos + m_front, m_up);
}

void Camera::translateRight(GLfloat val)
{
    m_pos += glm::normalize(glm::cross(m_front, m_up)) * val;
}

void Camera::translateLeft(GLfloat val)
{
    m_pos -= glm::normalize(glm::cross(m_front, m_up)) * val;
}

void Camera::translateForward(GLfloat val)
{
    m_pos += val * m_front;
}

void Camera::translateBack(GLfloat val)
{
    m_pos -= val * m_front;
}

void Camera::setFront(glm::vec3 front)
{
    m_front = front;
}

