#include <GL/glew.h>
#include <boost/format.hpp>

#include "render/shaderprogram.hpp"
#include "utils/utils.hpp"
#include "utils/logger.hpp"
#include "exceptions/glexception.hpp"

using boost::format;
using utils::log::Logger;
using utils::log::Category;
using utils::log::program_log_file_name;
using utils::log::shader_log_file_name;

ShaderProgram::ShaderProgram()
{
    m_programID = 0;
}

ShaderProgram::~ShaderProgram()
{
    freeProgram();
}

void ShaderProgram::freeProgram()
{
    glDeleteProgram(m_programID);
}

void ShaderProgram::bind() const
{
    glUseProgram(m_programID);
    if (GLenum error = glGetError(); error != GL_NO_ERROR) {
        utils::log::printProgramLog(m_programID);
        throw GLException((format("Unable to bind shader program! %s%\n") %
                           gluErrorString(error)).str(),
                          program_log_file_name(),
                          Category::INTERNAL_ERROR);
    }
}

void ShaderProgram::unbind()
{
    glUseProgram(0);
}

GLuint ShaderProgram::getProgramID()
{
    return m_programID;
}

void ShaderProgram::setInt(const std::string& name, GLint value)
{
    using utils::log::Logger;

    assert(!name.empty());
    GLint loc = glGetUniformLocation(m_programID, name.c_str());
    if (loc == -1)
        throw GLException((format("Unable to set uniform variable %1%\n") %
                           name).str(),
                          shader_log_file_name(),
                          Category::INTERNAL_ERROR);

    glUniform1i(loc, value);
    if (GLenum error = glGetError(); error != GL_NO_ERROR)
        throw GLException((format("Unable to set uniform variable \"%1%\"\n") % name).str(),
                          shader_log_file_name(),
                          Category::INTERNAL_ERROR);
}

void ShaderProgram::setFloat(const std::string &name, GLfloat value)
{
    using utils::log::Logger;

    assert(!name.empty());
    GLint loc = glGetUniformLocation(m_programID, name.c_str());
    if (loc == -1)
        throw GLException(
                (format("Can't find location by name \"%1%\"\n") % name).str(),
                shader_log_file_name(),
                Category::INTERNAL_ERROR);

    glUniform1f(loc, value);
    if (GLenum error = glGetError(); error != GL_NO_ERROR)
        throw GLException(
                (format("Unable to set uniform variable \"%1%\"\n") % name).str(),
                shader_log_file_name(),
                Category::INTERNAL_ERROR);
}

void ShaderProgram::setProjection(glm::mat4 matrix)
{
    m_projectionMatrix = matrix;
}

void ShaderProgram::setView(glm::mat4 matrix)
{
    m_viewMatrix = matrix;
}

void ShaderProgram::setModel(glm::mat4 matrix)
{
    m_modelMatrix = matrix;
}

glm::mat4 ShaderProgram::getView() const
{
    return m_viewMatrix;
}

glm::mat4 ShaderProgram::getProjection() const
{
    return m_projectionMatrix;
}

glm::mat4 ShaderProgram::getModel() const
{
    return m_modelMatrix;
}

void ShaderProgram::leftMultModel(glm::mat4 matrix)
{
    m_modelMatrix = matrix * m_modelMatrix;
}

void ShaderProgram::leftMultView(glm::mat4 matrix)
{
    m_viewMatrix = matrix * m_viewMatrix;
}

void ShaderProgram::leftMultProjection(glm::mat4 matrix)
{
    m_projectionMatrix = matrix * m_projectionMatrix;
}

void ShaderProgram::multModel(glm::vec4 vec)
{
    m_modelMatrix *= vec;
}

void ShaderProgram::multView(glm::vec4 vec)
{
    m_viewMatrix *= vec;
}

void ShaderProgram::multProjection(glm::vec4 vec)
{
    m_projectionMatrix *= vec;
}
