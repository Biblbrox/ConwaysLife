#include <GL/glew.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <boost/format.hpp>

#include "exceptions/sdlexception.hpp"
#include "render/sprite.hpp"
#include "utils/texture.hpp"

using utils::log::Category;
using utils::log::program_log_file_name;
using utils::log::shader_log_file_name;
using boost::format;

Sprite::Sprite()
{
    m_vao = nullptr;
    m_texCount = 0;
    m_curIdx = 0;
}

GLuint Sprite::addTexture(const std::string& objFile,
                          GLfloat textureWidth, GLfloat textureHeight,
                          GLfloat textureDepth)
{
    using namespace utils::texture;

    std::string textureFile;
    m_vertices.emplace_back(loadObj(objFile, textureFile));

    GLuint textureId = loadTexture(getResourcePath(textureFile),
                                          nullptr, nullptr);
    m_textureIds.emplace_back(textureId);

    utils::Rect rect;
    rect.w = textureWidth;
    rect.h = textureHeight;
    rect.d = textureDepth;
    m_sizes.emplace_back(rect);

    ++m_texCount;
}

utils::Rect Sprite::getClip(GLuint idx) noexcept
{
    assert(idx < m_texCount);
    return m_sizes[idx];
}

void Sprite::generateDataBuffer()
{
    if (!m_textureIds.empty()) {
        m_texCount = m_sizes.size();
        m_vao = new GLuint[m_texCount];

        glGenVertexArrays(m_texCount, m_vao);
        GLuint VBO;
//        GLuint EBO;

        for (GLuint i = 0; i < m_texCount; ++i) {

            GLfloat* vertices = &m_vertices[i][0];
            glBindVertexArray(m_vao[i]);
            glGenBuffers(1, &VBO);
//            glGenBuffers(1, &EBO);
//            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
//                         GL_STATIC_DRAW);

            size_t vertSize = m_vertices[i].size() * sizeof(GLfloat);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertSize, vertices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, // Pos of vertices
                                  5 * sizeof(GLfloat), nullptr);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), // UV coords
                                  (void*)(3 * sizeof(GLfloat)));
            glEnableVertexAttribArray(1);

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
//            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

//            glDeleteBuffers(1, &EBO);
            glDeleteBuffers(1, &VBO);
        }

    } else {
        if (m_textureId == 0)
            throw SdlException("No texture to generate from\n",
                               program_log_file_name(), Category::INTERNAL_ERROR);

        if (m_sizes.empty())
            throw SdlException("No data generate from\n",
                               program_log_file_name(), Category::INTERNAL_ERROR);
    }
}

void Sprite::freeVBO() noexcept
{
    if (m_vao) {
        glDeleteVertexArrays(m_texCount, m_vao);
        delete[] m_vao;
        m_vao = nullptr;
    }

    m_sizes.clear();
}

Sprite::~Sprite()
{
    freeVBO();
}

GLuint Sprite::getVAO() const
{
    return m_vao[m_curIdx];
}

GLuint Sprite::getIdx() const noexcept
{
    return m_curIdx;
}

utils::Rect Sprite::getCurrentClip() const noexcept
{
    return m_sizes[m_curIdx];
}

void Sprite::setIdx(GLuint idx)
{
    assert(idx < m_texCount);
    m_curIdx = idx;
}

GLuint Sprite::getWidth() const noexcept
{
    return m_sizes[m_curIdx].w;
}

GLuint Sprite::getHeight() const noexcept
{
    return m_sizes[m_curIdx].h;
}

GLuint Sprite::getSpritesCount() const noexcept
{
    return m_texCount;
}

GLuint Sprite::getTextureID() const
{
    return m_textureIds[m_curIdx];
}

GLuint Sprite::getDepth() const noexcept
{
    return m_sizes[m_curIdx].d;
}

