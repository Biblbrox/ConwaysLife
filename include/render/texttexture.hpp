#ifndef TEXTTEXTURE_HPP
#define TEXTTEXTURE_HPP

#include <SDL_ttf.h>

#include "texture.hpp"
#include "utils/utils.hpp"

class TextTexture: public Texture
{

public:
    TextTexture(const std::string& textureText, TTF_Font* font,
                SDL_Color color = {0x00, 0xFF, 0x00, 0xFF});
    ~TextTexture() override;

    void load(const std::string& textureText, SDL_Color color, TTF_Font* font);
    void setText(const std::string& text);
    void setFont(TTF_Font* font);
    void setColor(SDL_Color color);

    void generateDataBuffer() override;
    void freeVBO() noexcept override final;
    void freeFont() noexcept;

    GLuint getVAO() const override;
private:
    std::string m_text;
    TTF_Font* m_font;
    SDL_Color m_color;

    GLuint m_vaoId;
    GLuint m_vboId;
};

#endif //TEXTTEXTURE_HPP
