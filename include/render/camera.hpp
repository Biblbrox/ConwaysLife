#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Camera
{
public:
    Camera();
    glm::mat4 getView() const;

    GLfloat getX() const;
    GLfloat getY() const;

    void setX(GLfloat x);
    void setY(GLfloat y);
    void setFront(glm::vec3 front);

    GLfloat deltaX() const;
    GLfloat deltaY() const;

    void translateLeft(GLfloat val);
    void translateRight(GLfloat val);
    void translateForward(GLfloat val);
    void translateBack(GLfloat val);

private:
    glm::vec3 m_pos;
    glm::vec3 m_up;
    glm::vec3 m_front;
};

#endif //CAMERA_HPP