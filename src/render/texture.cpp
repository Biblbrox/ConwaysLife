#include <GL/glew.h>

#include "render/texture.hpp"

Texture::Texture() : m_textureId(0), m_textureWidth(0),
                     m_textureHeight(0),
                     m_textureDepth(0)
{
}

GLuint Texture::getWidth() const noexcept
{
    return m_textureWidth;
}

GLuint Texture::getHeight() const noexcept
{
    return m_textureHeight;
}

GLuint Texture::getDepth() const noexcept
{
    return m_textureDepth;
}

GLuint Texture::getTextureID() const
{
    return m_textureId;
}

void Texture::freeTexture()
{
    if (m_textureId != 0) {
        glDeleteTextures(1, &m_textureId);
        m_textureId = 0;
    }

    m_textureWidth = m_textureHeight = 0;
}

Texture::~Texture()
{
    freeTexture();
}

