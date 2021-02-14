#include <ctime>
#include <boost/format.hpp>
#include <iostream>
#include <fstream>

#include "utils/logger.hpp"
#include "utils/utils.hpp"
#include "exceptions/glexception.hpp"
#include "exceptions/fsexception.hpp"

using utils::log::Logger;
using utils::log::Category;
using utils::log::program_log_file_name;
using utils::log::shader_log_file_name;
using boost::format;
using glm::vec2;
using std::find_if;

void utils::padLine(std::string& line, size_t pad)
{
    line.insert(line.begin(), ' ', pad);
    line.insert(line.end(), ' ', pad);
}

GLuint utils::loadShaderFromFile(const std::string &path, GLenum shaderType)
{
    assert(!path.empty() && "Empty file path");
    GLuint shaderID = 0;
    std::string shaderString;
    std::ifstream sourceFile(path.c_str());
    if (!sourceFile.is_open())
        throw FSException((format("Can't open shader source file %1%\n")
                           % path).str(), program_log_file_name(),
                          utils::log::Category::FILE_ERROR);

    shaderString.assign(std::istreambuf_iterator<char>(sourceFile),
                        std::istreambuf_iterator<char>());

    shaderID = glCreateShader(shaderType);
    const GLchar *shaderSource = shaderString.c_str();
    glShaderSource(shaderID, 1, (const GLchar**) &shaderSource, NULL);
    glCompileShader(shaderID);

    GLint shaderCompiled = GL_FALSE;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shaderCompiled);
    if (shaderCompiled != GL_TRUE) {
        sourceFile.close();
        utils::log::printShaderLog(shaderID);
        glDeleteShader(shaderID);
        throw GLException(
                "Error while compiling shader(see shader log)!",
                program_log_file_name(),
                utils::log::Category::SHADER_COMPILE_ERROR);
    }

    sourceFile.close();
    return shaderID;
}

