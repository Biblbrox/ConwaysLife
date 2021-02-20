#ifndef SHADERPROGRAM_HPP
#define SHADERPROGRAM_HPP

#include <GL/glew.h>
#include <string>
#include <glm/mat4x4.hpp>

class ShaderProgram
{
public:
    explicit ShaderProgram();

    virtual ~ShaderProgram();
    virtual void loadProgram() = 0;

    virtual void setProjection(const glm::mat4& matrix);
    virtual void setModel(const glm::mat4& matrix);
    virtual void setView(const glm::mat4& matrix);

    virtual glm::mat4 getView() const;
    virtual glm::mat4 getModel() const;
    virtual glm::mat4 getProjection() const;

    virtual void leftMultView(const glm::mat4& matrix);
    virtual void leftMultModel(const glm::mat4& matrix);
    virtual void leftMultProjection(const glm::mat4& matrix);
    virtual void multModel(const glm::vec4& vec);
    virtual void multView(const glm::vec4& vec);
    virtual void multProjection(const glm::vec4& vec);

    virtual void updateProjection() = 0;
    virtual void updateView() = 0;
    virtual void updateModel() = 0;

    virtual void freeProgram() final;
    virtual void setFloat(const std::string& str, GLfloat value);
    virtual void setInt(const std::string &name, GLint value);
    virtual void setVec3(const std::string &name, const glm::vec3& value);
    virtual void setVec4(const std::string &name, const glm::vec4& value);

    void bind() const;
    void unbind();
    GLuint getProgramID();

protected:
    GLuint m_programID;

    glm::mat4 m_projectionMatrix;
    glm::mat4 m_modelMatrix;
    glm::mat4 m_viewMatrix;
};

#endif //SHADERPROGRAM_HPP
