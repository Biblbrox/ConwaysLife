#ifndef SPRITE_HPP
#define SPRITE_HPP

#include "texture.hpp"
#include "utils/utils.hpp"

/**
 * Sprite class.
 * Can contain one big texture with multiple small clips.
 */
class Sprite: public Texture
{
public:
    explicit Sprite();
    ~Sprite();
    void load(const std::string& path);
    GLuint addTexture(const std::string& objFile,
                      GLfloat textureWidth,
                      GLfloat textureHeight, GLfloat textureDepth);
    utils::Rect getClip(GLuint idx) noexcept;

    GLuint getWidth() const noexcept override;
    GLuint getHeight() const noexcept override;
    GLuint getDepth() const noexcept override;
    utils::Rect getCurrentClip() const noexcept;
    GLuint getIdx() const noexcept;
    void setIdx(GLuint idx);
    GLuint getSpritesCount() const noexcept;
    GLuint getTextureID() const override;

    void generateDataBuffer() override;
    void freeVBO() noexcept override final;

    GLuint getVAO() const override;
protected:
    std::vector<utils::Rect> m_sizes;

    GLuint* m_vao;
    GLuint m_texCount;
    GLuint m_curIdx = 0;

    std::vector<std::vector<GLfloat>> m_vertices;
    std::vector<GLuint> m_textureIds;
};

#endif //SPRITE_HPP
