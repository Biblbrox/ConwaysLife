#include <GL/glew.h>
#include <string>
#include <boost/format.hpp>
#include <SDL_ttf.h>

#include "exceptions/sdlexception.hpp"
#include "render/texttexture.hpp"
#include "utils/texture.hpp"

using boost::format;

void TextTexture::setText(const std::string& text)
{
    load(text, m_color, m_font);
    m_text = text;
}

void TextTexture::setFont(TTF_Font* font)
{
    load(m_text, m_color, font);
    m_font = font;
}

void TextTexture::setColor(SDL_Color color)
{
    load(m_text, color, m_font);
    m_color = color;
}

TextTexture::TextTexture(const std::string& textureText,
                         TTF_Font *font, SDL_Color color)
{
    assert(font);

    m_font = font;
    m_color = color;
    m_text = textureText;
    m_vaoId = 0;
    m_textureId = 0;
    load(textureText, m_color, m_font);
}

void TextTexture::load(const std::string &textureText, SDL_Color color,
                       TTF_Font* font)
{
    using namespace utils::texture;
    assert(!textureText.empty());

    if (m_textureId != 0 && m_text == textureText)
        return;

    size_t old_width = m_textureWidth;

    freeTexture();
    std::vector<std::string> lines = utils::split_to_lines(textureText);
    std::string maxLenStr = *std::max_element(
            lines.begin(), lines.end(),
            [](const std::string& first, const std::string& second){
                return first.size() < second.size();
            });

    // Center lines
    for (std::string& line: lines)
        utils::padLine(line, (line.size() - maxLenStr.size()) / 2);

    int width;
    TTF_SizeText(font, maxLenStr.c_str(), &width, nullptr);
    SDL_Surface* surface =
            TTF_RenderText_Blended_Wrapped(font, textureText.c_str(), color,
                                           width);

    if (!surface)
        throw SdlException((format("Unable to create blended text. "
                                   "SDL_ttf Error: %s\n")
                            % TTF_GetError()).str());

    SDL_Surface* flipped = flipVertically(surface);
    SDL_FreeSurface(surface);
    if (!flipped)
        throw SdlException((format("Unable to flip surface %p\n") % surface).str());

    GLenum texture_format = getSurfaceFormatInfo(*flipped);

    m_textureWidth = flipped->w;
    m_textureHeight = flipped->h;

    m_textureId = loadTextureFromPixels32
            (static_cast<GLuint*>(flipped->pixels),
             m_textureWidth, m_textureHeight, texture_format);
    SDL_FreeSurface(flipped);

    if (m_textureId != 0 && m_vaoId != 0
        && m_textureWidth != old_width) { // Width update
        GLfloat quadWidth = m_textureWidth;
        GLfloat quadHeight = m_textureHeight;

        GLfloat vertices[] = {
                // positions            // texture coords
                quadWidth,  0.0f,       1.0f, 1.0f, // top right
                quadWidth, quadHeight,  1.0f, 0.0f, // bottom right
                0.f, quadHeight,        0.0f, 0.0f, // bottom left
                0.f,  0.f,              0.0f, 1.0f  // top left
        };
        glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    generateDataBuffer();
}

void TextTexture::generateDataBuffer()
{
    if (m_textureId != 0 && m_vaoId == 0) {
        GLfloat quadWidth = m_textureWidth;
        GLfloat quadHeight = m_textureHeight;

        GLfloat vertices[] = {
                // positions            // texture coords
                quadWidth,  0.0f,       1.0f, 1.0f, // top right
                quadWidth, quadHeight,  1.0f, 0.0f, // bottom right
                0.f, quadHeight,        0.0f, 0.0f, // bottom left
                0.f,  0.f,              0.0f, 1.0f  // top left
        };

        GLuint indices[] = {
                3, 1, 0,
                3, 2, 1
        };

        GLuint ibo_id;

        glGenVertexArrays(1, &m_vaoId);
        glGenBuffers(1, &m_vboId);
        glGenBuffers(1, &ibo_id);

        glBindVertexArray(m_vaoId);

        glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                     GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                     GL_STATIC_DRAW);

        //position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                              4 * sizeof(GLfloat), nullptr);
        glEnableVertexAttribArray(0);

        //Tex coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                              4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glDeleteBuffers(1, &ibo_id);
    }
}

void TextTexture::freeVBO() noexcept
{
    if (m_vaoId != 0) {
        glDeleteVertexArrays(1, &m_vaoId);
        glDeleteBuffers(1, &m_vboId);
    }
}

TextTexture::~TextTexture()
{
    freeFont();
    freeVBO();
}

GLuint TextTexture::getVAO() const
{
    return m_vaoId;
}

void TextTexture::freeFont() noexcept
{
    if (m_font)
        TTF_CloseFont(m_font);
}
