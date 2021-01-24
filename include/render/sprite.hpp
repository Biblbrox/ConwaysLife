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
    explicit Sprite(const std::string& path);
    ~Sprite();
    void load(const std::string& path);
    GLuint addClipSprite(utils::Rect clip);
    utils::Rect getClip(GLuint idx) noexcept;

    GLuint getWidth() const noexcept override;
    GLuint getHeight() const noexcept override;
    utils::Rect getCurrentClip() const noexcept;
    GLuint getIdx() const noexcept;
    void setIdx(GLuint idx);
    GLuint getSpritesCount() const noexcept;

    void generateDataBuffer() override;
    void freeVBO() noexcept override final;

    GLuint getVAO() const override;
protected:
    std::vector<utils::Rect> m_clips;

    GLuint* m_vao;
    GLuint m_totSprites;
    GLuint m_curIdx = 0;
};

#endif //SPRITE_HPP
