#include <GL/glew.h>
#include <cassert>
#include <vector>
#include <string>
#include <filesystem>
#include <boost/format.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <SDL_image.h>
#include <fstream>

#include "utils/texture.hpp"
#include "utils/logger.hpp"
#include "utils/string.hpp"
#include "constants.hpp"
#include "exceptions/fsexception.hpp"
#include "exceptions/sdlexception.hpp"
#include "exceptions/glexception.hpp"

using boost::format;
using utils::string::split;

std::vector<GLfloat>
utils::texture::loadObj(const std::string& file, std::string& textureFile)
{
    // TODO: write more general cache
    static std::vector<GLfloat> last_result = {};
    static std::string last_file = "";
    static std::string last_tex_file = "";

    if (!last_result.empty() && last_file == file) {
        textureFile = last_tex_file;
        return last_result;
    }

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
//                size_t normalIdx = std::stol(idx[1]);
                size_t textureIdx = std::stol(idx[1]);
                indices.emplace_back(vertexIdx - 1, 0, textureIdx - 1);
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
    const std::string mtlPath = getResourcePath(mtlFile);
    if (!std::filesystem::exists(getResourcePath(mtlPath)))
        throw FSException((format("File: %s doesn't exists") % mtlPath).str(),
                          program_log_file_name(),
                          utils::log::Category::FILE_ERROR);

    std::ifstream mtl(getResourcePath(mtlPath));
    while (std::getline(mtl, line)) {
        if (line.starts_with("map_Kd")) {
            textureFile = line.substr(line.find("map_Kd") + 6);
            boost::trim(textureFile);
            break;
        }
    }
    mtl.close();

    last_result = res;
    last_file = file;
    last_tex_file = textureFile;

    return res;
}

GLuint utils::texture::loadTexture(const std::string &file,
                                   GLuint *textureWidth,
                                   GLuint *textureHeight) {
    using namespace utils::texture;

    static std::string old_file;
    static GLuint old_texture;
    if (!old_file.empty() && old_file == file)
        return old_texture;

    SDL_Surface* surface = IMG_Load(file.c_str());
    if (!surface)
        throw SdlException((format("Unable to load image: %s"
                                   ". SDL Error: %s\n")
                            % file % SDL_GetError()).str());
    SDL_Surface* flipped = flipVertically(surface);
    SDL_FreeSurface(surface);
    if (!flipped)
        throw SdlException((format("Unable to flip surface %p\n") % surface).str());

    GLenum texture_format = getSurfaceFormatInfo(*flipped);

    GLuint tw = flipped->w;
    GLuint th = flipped->h;

    if (textureWidth)
        *textureWidth = tw;
    if (textureHeight)
        *textureHeight = th;

    GLuint textureId = loadTextureFromPixels32(
            static_cast<GLuint*>(flipped->pixels),
            tw, th, texture_format);

    SDL_FreeSurface(flipped);

    old_file = file;
    old_texture = textureId;

    return textureId;
}

GLuint
utils::texture::loadTextureFromPixels32(const GLuint *pixels, GLuint width, GLuint height,
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