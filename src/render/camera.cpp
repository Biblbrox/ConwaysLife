#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "render/camera.hpp"
#include "utils/utils.hpp"
#include "config.hpp"
#include "utils/math.hpp"

using utils::math::sgn;

std::shared_ptr<Camera> Camera::instance = nullptr;

Camera::Camera(const glm::vec3& pos, const glm::vec3& target,
               const glm::vec3& up) : m_pos(pos), m_target(target), m_up(up),
                                      m_mouseSens(initSens), m_zoom(initZoom)
{
    updateView();
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

glm::mat4 Camera::getView()
{
    return m_view;
}

void Camera::setPos(const glm::vec3& pos)
{
    m_pos = pos;
}

glm::vec3 Camera::getPos() const
{
    return m_pos;
}

void Camera::processMovement(GLfloat xoffset, GLfloat yoffset)
{
    int sign = -1;
    if (Config::getVal<bool>("InverseRotation"))
        sign = 1;

    xoffset *= sign * m_mouseSens;
    yoffset *= sign * m_mouseSens;

    m_xOffset = xoffset;
    m_yOffset = yoffset;

    updateView();
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

void Camera::updateView()
{
    glm::vec4 position = glm::vec4(m_pos, 1.f);
    glm::vec4 pivot = glm::vec4(m_target, 1.f);

    int screen_width = utils::getDisplayWidth<int>();
    int screen_height = utils::getDisplayHeight<int>();

    GLfloat deltaAngleX = (2 * M_PI / screen_width);
    GLfloat deltaAngleY = (M_PI / screen_height);
    GLfloat xAngle = m_xOffset * deltaAngleX;
    GLfloat yAngle = m_yOffset * deltaAngleY;

    GLfloat cosAngle = glm::dot(getViewDir(), m_up);
    if (std::abs(cosAngle) > 0.99f && sgn(yAngle) == sgn(cosAngle))
        yAngle = 0;

    glm::mat4 rotationMatrixX = glm::rotate(glm::mat4(1.f), xAngle, m_up);
    position = (rotationMatrixX * (position - pivot)) + pivot;

    glm::mat4 rotationMatrixY = glm::rotate(glm::mat4(1.f), yAngle, getRightVec());
    glm::vec3 finalPos = (rotationMatrixY * (position - pivot)) + pivot;

    m_pos = finalPos;
    m_view = glm::lookAt(m_pos, m_target, m_up);
}

glm::vec3 Camera::getViewDir() const
{
    return -glm::transpose(m_view)[2];
}

glm::vec3 Camera::getRightVec() const
{
    return glm::transpose(m_view)[0];
}

glm::vec3 Camera::getTarget() const
{
    return m_target;
}

glm::vec3 Camera::getUp() const
{
    return m_up;
}

void Camera::setView(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up)
{
    m_pos = pos;
    m_target = target;
    m_up = up;
    updateView();
}
