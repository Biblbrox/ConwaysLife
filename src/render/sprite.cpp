#include <GL/glew.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <boost/format.hpp>

#include "exceptions/sdlexception.hpp"
#include "render/sprite.hpp"

using utils::log::Category;
using utils::log::program_log_file_name;
using utils::log::shader_log_file_name;
using boost::format;

Sprite::Sprite(const std::string& path)
{
    load(path);
    m_vao = nullptr;
    m_totSprites = 0;
    m_curIdx = 0;
}

void Sprite::load(const std::string& path)
{
    assert(!path.empty());
    freeTexture();
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface)
        throw SdlException((format("Unable to create blended text. "
                                   "SDL_ttf Error: %s\n")
                            % TTF_GetError()).str());
    SDL_Surface* flipped = utils::flipVertically(surface);
    SDL_FreeSurface(surface);
    if (!flipped)
        throw SdlException((format("Unable to flip surface %p\n") % surface).str());

    GLenum texture_format = utils::getSurfaceFormatInfo(*flipped);

    m_textureWidth = flipped->w;
    m_textureHeight = flipped->h;

    m_textureId = utils::loadTextureFromPixels32(
            static_cast<GLuint*>(flipped->pixels),
            m_textureWidth, m_textureHeight, texture_format);
    SDL_FreeSurface(flipped);
}

GLuint Sprite::addClipSprite(utils::Rect clip)
{
    m_clips.push_back(clip);
    return m_clips.size() - 1;
}

utils::Rect Sprite::getClip(GLuint idx) noexcept
{
    assert(idx < m_totSprites);
    return m_clips[idx];
}

void Sprite::generateDataBuffer()
{
    if (m_textureId != 0 && !m_clips.empty()) {
        m_totSprites = m_clips.size();
        m_vao = new GLuint[m_totSprites];

        glGenVertexArrays(m_totSprites, m_vao);
        GLuint VBO;
        GLuint EBO;
        GLfloat tw = m_textureWidth;
        GLfloat th = m_textureHeight;

//        GLuint indices[] = {
//                3, 1, 0,
//                3, 2, 1
//        };

//        GLuint indices[] = {
//                 // Back edge
//
//
//        };
//
        GLfloat vertices[] = {
                -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
                0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
                0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
                -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
                0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
                -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

                -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

                0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

                -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
                0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

                -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
                0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
                -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
        };
        for (GLuint i = 0; i < m_totSprites; ++i) {
//            vertices[0] = 0.5f; // Right bottom x // 1 row
//            vertices[1] = -0.5f; // Right bottom y // 1 row
//            vertices[2] = (m_clips[i].x + m_clips[i].w) / tw; // 1 row
//            vertices[3] = (m_clips[i].y + m_clips[i].h) / th; // 1 row
//
//            vertices[4] = 0.5f; // Right top x // 2 row
//            vertices[5] = 0.5f; // Right top y // 2 row
//            vertices[6] = (m_clips[i].x + m_clips[i].w) / tw; // 2 row
//            vertices[7] = m_clips[i].y / th; // 2 row
//
//            vertices[8] = -0.5f; // Left top x // 3 row
//            vertices[9] = 0.5f; // Left top y // 3 row
//            vertices[10] = m_clips[i].x / tw; // 3 row
//            vertices[11] = m_clips[i].y / th; // 3 row
//
//            vertices[12] = -0.5f; // Bottom left x // 4 row
//            vertices[13] = -0.5f; // Bottom left y // 4 row
//            vertices[14] = m_clips[i].x / tw; // 4 row
//            vertices[15] = (m_clips[i].y + m_clips[i].h) / th; // 4 row

            glBindVertexArray(m_vao[i]);
            glGenBuffers(1, &VBO);
//            glGenBuffers(1, &EBO);
//            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
//                         GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                         GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, // Pos of vertices
                                  5 * sizeof(GLfloat), nullptr);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), // Texture pos
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
            throw SdlException("No texture to render\n",
                               program_log_file_name(), Category::INTERNAL_ERROR);

        if (m_clips.empty())
            throw SdlException("No data generate from\n",
                               program_log_file_name(), Category::INTERNAL_ERROR);
    }
}

void Sprite::freeVBO() noexcept
{
    if (m_vao) {
        glDeleteVertexArrays(m_totSprites, m_vao);
        delete[] m_vao;
        m_vao = nullptr;
    }

    m_clips.clear();
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
    return m_clips[m_curIdx];
}

void Sprite::setIdx(GLuint idx)
{
    assert(idx < m_totSprites);
    m_curIdx = idx;
}

GLuint Sprite::getWidth() const noexcept
{
    return m_clips[m_curIdx].w;
}

GLuint Sprite::getHeight() const noexcept
{
    return m_clips[m_curIdx].h;
}

GLuint Sprite::getSpritesCount() const noexcept
{
    return m_totSprites;
}

