#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include <GL/glew.h>

/**
 * Texture class.
 * This is only base methods. You can't create instance of this class
 */
class Texture
{
public:
    virtual ~Texture();

    virtual GLuint getTextureID() const;

    virtual GLuint getWidth() const noexcept;
    virtual GLuint getHeight() const noexcept;
    virtual GLuint getDepth() const noexcept;

    virtual GLuint getVAO() const = 0;
    virtual void generateDataBuffer() = 0;

    virtual void freeTexture() final;
    virtual void freeVBO() noexcept = 0;

protected:
    explicit Texture();

    GLuint m_textureId;

    GLuint m_textureWidth;
    GLuint m_textureHeight;
    GLuint m_textureDepth;
};

#endif //TEXTURE_HPP
