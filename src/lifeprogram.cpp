#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "utils/utils.hpp"
#include "boost/format.hpp"
#include "exceptions/glexception.hpp"
#include "utils/logger.hpp"
#include "lifeprogram.hpp"

using utils::getShaderPath;
using utils::loadShaderFromFile;
using glm::mat4;
using glm::vec3;
using glm::vec4;
using utils::log::Logger;
using utils::log::Category;
using utils::log::program_log_file_name;
using utils::log::shader_log_file_name;
using boost::format;

const int gl_bool_size = 4;
const int gl_int_size = 4;
const int gl_float_size = 4;

constexpr int next_offset(int cur_offset, int base_alignment)
{
    if (cur_offset % base_alignment == 0)
        return cur_offset;

    return cur_offset + base_alignment - cur_offset % base_alignment;
}

const char* textureNumberGL = "TextureNum";

LifeProgram::LifeProgram()
{
    m_isTextureRender = false;
    m_texLoc = 0;
    m_colorLoc = 0;
}

void LifeProgram::loadProgram()
{
    m_programID = glCreateProgram();
    if (m_programID == 0)
        throw GLException((format("Unable to create program %d\n")
                           % m_programID).str(),
                          program_log_file_name(),
                          Category::INITIALIZATION_ERROR);

    // Vertex shader
    m_vertexShader = loadShaderFromFile(
            getShaderPath("LifeGame.glvs"), GL_VERTEX_SHADER);
    // Vertex shader end

    // Fragment shader
    m_fragmentShader = loadShaderFromFile(
            getShaderPath("LifeGame.glfs"), GL_FRAGMENT_SHADER);
    // Fragment shader end

    glAttachShader(m_programID, m_fragmentShader);
    glAttachShader(m_programID, m_vertexShader);
    if (GLenum error = glGetError(); error != GL_NO_ERROR) {
        utils::log::printProgramLog(m_programID);
        throw GLException("Unable to attach shaders.\n",
                          program_log_file_name(),
                          Category::INITIALIZATION_ERROR);
    }

    glLinkProgram(m_programID);
    GLint linkSuccess = GL_TRUE;
    glGetProgramiv(m_programID, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess != GL_TRUE) {
        utils::log::printProgramLog(m_programID);
        throw GLException((format("Unable to link program: %d, %s\n")
                           % m_programID % glGetError()).str(),
                          program_log_file_name(),
                          Category::INITIALIZATION_ERROR);
    }

    // Get block indices
    GLuint matrixLoc = glGetUniformBlockIndex(m_programID, "Matrices");

    // Link shader's uniform block to uniform binding point
    glUniformBlockBinding(m_programID, matrixLoc, 0); // Matrices

    // Create matrices buffer
    glGenBuffers(1, &m_matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(mat4), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    // Bind matrices to 0 index in binding point array
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_matricesUBO);

    glUseProgram(m_programID);
    rebindUniforms();
    setTexture(0);
}

void LifeProgram::setTexture(int texture)
{
    glUniform1i(m_texLoc, texture);
}

LifeProgram::~LifeProgram()
{
    remove_shaders();
    remove_programs();
    free_buffers();
}

std::shared_ptr<LifeProgram> LifeProgram::instance = nullptr;

void LifeProgram::rebindUniforms()
{
    m_texLoc = glGetUniformLocation(m_programID, textureNumberGL);
    if (m_texLoc == -1) {
        utils::log::printProgramLog(m_programID);
        throw GLException((format("%s is not a valid glsl program variable!\n")
                           % textureNumberGL).str(),
                          program_log_file_name(),
                          Category::INTERNAL_ERROR);
    }
}

void LifeProgram::remove_programs()
{
    if (glIsProgram(m_programID)) {
        glDeleteProgram(m_programID);
        m_programID = 0;
    }
}

void LifeProgram::free_buffers()
{
    glDeleteBuffers(1, &m_matricesUBO);
    glDeleteBuffers(1, &m_textureDataUBO);

    m_matricesUBO = m_textureDataUBO = 0;
}

void LifeProgram::remove_shaders()
{
    if (glIsShader(m_fragmentShader))
        glDeleteShader(m_fragmentShader);

    if (glIsShader(m_vertexShader))
        glDeleteShader(m_vertexShader);
}

void LifeProgram::updateProjection()
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0,
                    sizeof(glm::mat4), glm::value_ptr(m_projectionMatrix));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    if (GLenum error = glGetError(); error != GL_NO_ERROR) {
        utils::log::printProgramLog(m_programID);
        throw GLException((format("Error while updating matricesUBO(m_projectionMatrix)! %s\n")
                           % gluErrorString(error)).str(),
                          program_log_file_name(),
                          Category::INTERNAL_ERROR);
    }
}

void LifeProgram::updateModel()
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4),
                    sizeof(glm::mat4), glm::value_ptr(m_modelMatrix));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    if (GLenum error = glGetError(); error != GL_NO_ERROR) {
        utils::log::printProgramLog(m_programID);
        throw GLException((format("Error while updating matricesUBO(m_modelMatrix)! %s\n")
                           % gluErrorString(error)).str(),
                          program_log_file_name(),
                          Category::INTERNAL_ERROR);
    }
}

void LifeProgram::updateView()
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4),
                    sizeof(mat4), glm::value_ptr(m_viewMatrix));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    if (GLenum error = glGetError(); error != GL_NO_ERROR) {
        utils::log::printProgramLog(m_programID);
        throw GLException((format("Error while updating"
                                  " matricesUBO(m_viewMatrix)! %s\n")
                           % gluErrorString(error)).str(),
                          program_log_file_name(),
                          Category::INTERNAL_ERROR);
    }
}
