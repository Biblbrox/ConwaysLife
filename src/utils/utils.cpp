#include <ctime>
#include <boost/format.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <SDL_image.h>

#include "utils/logger.hpp"
#include "utils/utils.hpp"
#include "exceptions/glexception.hpp"
#include "exceptions/fsexception.hpp"
#include "exceptions/sdlexception.hpp"

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

GLuint
utils::loadTextureFromPixels32(const GLuint *pixels, GLuint width, GLuint height,
                               GLenum texture_format)
{
    assert(pixels);
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, texture_format,
                 GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    if (GLuint error = glGetError(); error != GL_NO_ERROR)
        throw GLException((format("Error loading texture from %1% pixels! %2%\n")
                           % pixels % gluErrorString(error)).str(),
                          program_log_file_name(), Category::INTERNAL_ERROR);

    return textureID;
}

GLfloat
utils::physics::altitude(const std::vector<vec2>& line_points,
                              GLfloat x, GLfloat y)
{
    assert(line_points.size() >= 2 && "line_points size must be >= 2");
    size_t line_idx = std::prev(std::lower_bound(line_points.cbegin(), line_points.cend(),
                                       x, [](const vec2& point, GLfloat val) {
                return point.x < val;
    })) - line_points.cbegin();

    const GLfloat curX = line_points[line_idx].x;
    const GLfloat nextX = line_points[line_idx + 1].x;
    const GLfloat curY = line_points[line_idx].y;
    const GLfloat nextY = line_points[line_idx + 1].y;

    return (x - curX) / (nextX - curX) * (nextY - curY) + curY - y;
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

std::vector<std::string> split(const std::string& str, const std::string& del)
{
    std::string s(str);
    size_t pos = 0;
    std::string token;
    std::vector<std::string> res;
    while (s.starts_with(del)) s.erase(0, del.length());
    while ((pos = s.find(del)) != std::string::npos) {
        res.emplace_back(s.substr(0, pos));
        s.erase(0, pos + del.length());
    }
    res.emplace_back(s);

    return res;
}

std::vector<GLfloat> utils::loadObj(const std::string& file, std::string& textureFile)
{
    if (!std::filesystem::exists(file))
        throw FSException((format("File %s doesn't exists") % file).str(),
                          program_log_file_name(),
                          utils::log::Category::FILE_ERROR);

    std::ifstream obj(file);
    std::string line;
    std::string mtlFile;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> indices;
    while (std::getline(obj, line)) {
        if (line.starts_with("vt")) {
            GLfloat x, y;
            std::string data = line.substr(line.find(' ')); // Skip "vt"
            std::istringstream s(data);
            s >> x >> y;
            uv.emplace_back(x, y);
        } else if (line.starts_with("v") && !line.starts_with("vn")) {
            GLfloat x, y, z;
            std::string data = line.substr(line.find(' ')); // Skip "v"
            std::istringstream s(data);
            s >> x >> y >> z;
            vertices.emplace_back(x, y, z);
        } else if (line.starts_with("f")) {
            std::string data = line.substr(line.find(' ')); // Skip "f"

            std::vector<std::string> parts = split(data, " ");
            for (auto& str: parts) {
                std::vector<std::string> idx = split(str, "/");
                size_t vertexIdx = std::stol(idx[0]);
                size_t normalIdx = std::stol(idx[1]);
                size_t textureIdx = std::stol(idx[2]);
                indices.emplace_back(vertexIdx - 1, normalIdx - 1, textureIdx - 1);
            }
        } else if (line.starts_with("mtllib")) {
            mtlFile = line.substr(line.find(' '));
            boost::trim(mtlFile);
        }
    }
    obj.close();

    std::vector<GLfloat> res;
    for (const glm::vec3& data: indices) {
        res.emplace_back(vertices[data[0]].x); // Vertex coords
        res.emplace_back(vertices[data[0]].y);
        res.emplace_back(vertices[data[0]].z);
        res.emplace_back(uv[data[1]].x); // UV coords
        res.emplace_back(uv[data[1]].y);
    }

    // Extract texture file name from mtl file
    const std::string mtlPath = utils::getResourcePath(mtlFile);
    if (!std::filesystem::exists(utils::getResourcePath(mtlPath)))
        throw FSException((format("File: %s doesn't exists") % mtlPath).str(),
                          program_log_file_name(),
                          utils::log::Category::FILE_ERROR);


    std::ifstream mtl(utils::getResourcePath(mtlPath));
    while (std::getline(mtl, line)) {
        if (line.starts_with("map_Kd")) {
            textureFile = line.substr(line.find("map_Kd") + 6);
            boost::trim(textureFile);
            break;
        }
    }
    mtl.close();

    return res;
}

GLuint utils::loadTexture(const std::string& file,
                          GLuint* textureWidth, GLuint* textureHeight)
{
    SDL_Surface* surface = IMG_Load(file.c_str());
    if (!surface)
        throw SdlException((format("Unable to load image: %s"
                                   ". SDL Error: %s\n")
                            % file % SDL_GetError()).str());
    SDL_Surface* flipped = utils::flipVertically(surface);
    SDL_FreeSurface(surface);
    if (!flipped)
        throw SdlException((format("Unable to flip surface %p\n") % surface).str());

    GLenum texture_format = utils::getSurfaceFormatInfo(*flipped);

    GLuint tw = flipped->w;
    GLuint th = flipped->h;

    if (textureWidth)
        *textureWidth = tw;
    if (textureHeight)
        *textureHeight = th;

    GLuint textureId = utils::loadTextureFromPixels32(
            static_cast<GLuint*>(flipped->pixels),
            tw, th, texture_format);

    SDL_FreeSurface(flipped);

    return textureId;
}
