#include <GL/glew.h>
#include <boost/format.hpp>
#include <iostream>

#include "exceptions/sdlexception.hpp"
#include "render/sprite.hpp"
#include "utils/texture.hpp"

using utils::log::Category;
using utils::log::program_log_file_name;
using utils::log::shader_log_file_name;
using boost::format;

Sprite::Sprite() : m_vao(nullptr)
{
}

void Sprite::addTexture(const std::string& objFile,
                          GLfloat textureWidth, GLfloat textureHeight,
                          GLfloat textureDepth)
{
    using namespace utils::texture;

//    std::string textureFile;
    m_vertices.emplace_back(std::vector<GLfloat>{
//            1.000000, 1.000000, -1.000000,
//            1.000000, -1.000000, -1.000000,
//            1.000000, 1.000000, 1.000000,
//            1.000000, -1.000000, 1.000000,
//            -1.000000, 1.000000, -1.000000,
//            -1.000000, -1.000000, -1.000000,
//            -1.000000, 1.000000, 1.000000,
//            -1.000000, -1.000000, 1.000000,
//            0.948500, 1.000000, -0.948500,
//            -0.948500, 1.000000, -0.948500,
//            -0.948500, 1.000000, 0.948500,
//            0.948500, 1.000000, 0.948500,
//            0.948500, -0.948500, 1.000000,
//            0.948500, 0.948500, 1.000000,
//            -0.948500, 0.948500, 1.000000,
//            -0.948500, -0.948500, 1.000000,
//            -1.000000, -0.948500, 0.948500,
//            -1.000000, 0.948500, 0.948500,
//            -1.000000, 0.948500, -0.948500,
//            -1.000000, -0.948500, -0.948500,
//            -0.948500, -1.000000, -0.948500,
//            0.948500, -1.000000, -0.948500,
//            0.948500, -1.000000, 0.948500,
//            -0.948500, -1.000000, 0.948500,
//            1.000000, -0.948500, -0.948500,
//            1.000000, 0.948500, -0.948500,
//            1.000000, 0.948500, 0.948500,
//            1.000000, -0.948500, 0.948500,
//            -0.948500, -0.948500, -1.000000,
//            -0.948500, 0.948500, -1.000000,
//            0.948500, 0.948500, -1.000000,
//            0.948500, -0.948500, -1.000000,
//            1.000000, -1.000000, 1.000000,
//            1.000000, -0.948500, -0.948500,
//            0.948500, 1.000000, 0.948500
            1.000000, 1.000000, -1.000000,
            1.000000, -1.000000, -1.000000,
            1.000000, 1.000000, 1.000000,
            1.000000, -1.000000, 1.000000,
            -1.000000, 1.000000, -1.000000,
            -1.000000, -1.000000, -1.000000,
            -1.000000, 1.000000, 1.000000,
            -1.000000, -1.000000, 1.000000
    });
//    m_vertices.emplace_back(loadObj(objFile, textureFile));
//
//    GLuint textureId = loadTexture(getResourcePath(textureFile),
//                                          nullptr, nullptr);
    m_textureIds.emplace_back(1);

    m_sizes.emplace_back(textureWidth, textureHeight, textureDepth);
}

glm::vec3 Sprite::getClip(GLuint idx) noexcept
{
    assert(idx < m_sizes.size());
    return m_sizes[idx];
}

void Sprite::generateDataBuffer()
{
    if (!m_textureIds.empty()) {
        size_t texCount = m_sizes.size();
        m_vao = new GLuint[texCount];
        GLuint indices[] = {
//                15, 13, 14, // с единицы
//                19, 17, 18,
//                24, 21, 22,
//                9, 5, 10,
//                10, 7 ,11,
//                11, 3 ,12,
//                12, 1 ,9,
//                13, 3 ,14,
//                14, 7 ,15,
//                15, 8 ,16,
//                16, 4 ,13,
//                17, 7 ,18,
//                18, 5 ,19,
//                19, 6 ,20,
//                20, 8 ,17,
//                21, 2 ,22,
//                22, 4 ,23,
//                23, 8 ,24,
//                24, 6 ,21,
//                25, 1 ,26,
//                26, 3 ,27,
//                27, 4 ,28,
//                28, 2 ,25,
//                29, 5 ,30,
//                30, 1 ,31,
//                31, 2 ,32,
//                32, 6 ,29,
//                1 ,32 ,2,
//                32, 25, 2,
//                25, 29, 2,
//                23, 2 ,22,
//                29, 5 ,6,
//                5 ,20 ,6,
//                1 ,25 ,32,
//                25, 29, 32,
//                1 ,31 ,25,
//                31, 26, 25,
//                30, 5 ,29,
//                5 ,19 ,20,
//                1 ,26 ,31,
//                26, 30, 31,
//                1 ,30 ,26,
//                1, 5, 30,
//                29, 6 ,2,
//                23, 4 ,2,
//                4 ,25 ,2,
//                25, 1 ,2,
//                12, 34, 35,
//                9 ,26 ,25,
//                26, 1 ,25,
//                5 , 7, 11,
//                26, 1 ,9,
//                26, 3 ,1,
//                12, 3 ,26,
//                14, 4 ,13,
//                14, 3 ,4,
//                3 ,12 ,4,
//                11, 7 ,3,
//                5 ,11 ,10,
//                10, 11, 12,
//                12, 11, 3,
//                12, 3 ,1,
//                10, 12, 9,
//                9, 12 ,1,
//                5, 10 ,9,
//                1, 5, 9,
//                4 ,35 ,33,
//                25, 33, 34,
//                29, 2 ,32,
//                30, 29, 32,
//                30, 32, 31,
//                31, 32, 1,
//                5 ,30 ,31,
//                5 ,31 ,1,
//                3 ,4, 28,
//                28, 4 ,2,
//                28, 2 ,25,
//                3 ,28 ,27,
//                27, 28, 25,
//                27, 25, 26,
//                3 ,27 ,26,
//                19, 20, 17,
//                24, 22, 23,
//                15, 16, 13,
//                9 ,1, 5,
//                10, 5 ,7,
//                11, 7 ,3,
//                12, 3 ,1,
//                13, 4 ,3,
//                14, 3 ,7,
//                15, 7 ,8,
//                16, 8 ,4,
//                17, 8 ,7,
//                18, 7 ,5,
//                19, 5 ,6,
//                20, 6 ,8,
//                21, 6 ,2,
//                22, 2 ,4,
//                23, 4 ,8,
//                24, 8 ,6,
//                25, 2 ,1,
//                26, 1 ,3,
//                27, 3 ,4,
//                28, 4 ,2,
//                29, 6 ,5,
//                30, 5 ,1,
//                31, 1 ,2,
//                32, 2 ,6,
//                12, 25, 34,
//                4 ,12 ,35,
//                25, 4 ,33
                5, 3, 1,
                3, 8, 4,
                7, 6, 8,
                2, 8, 6,
                1, 4, 2,
                5, 2, 6,
                5, 7, 3,
                3, 7, 8,
                7, 5, 6,
                2, 4, 8,
                1, 3, 4,
                5, 1, 2
        };

        for (unsigned int & idx : indices)
            idx -= 1;

        glGenVertexArrays(texCount, m_vao);

        GLuint VBO;
        GLuint EBO;

        for (GLuint i = 0; i < texCount; ++i) {

            GLfloat* vertices = &m_vertices[i][0];
            glBindVertexArray(m_vao[i]);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            size_t vertSize = m_vertices[i].size() * sizeof(GLfloat);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertSize, vertices, GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                         GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, // Pos of vertices
                                  3 * sizeof(GLfloat), nullptr);
            glEnableVertexAttribArray(0);

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
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
        glDeleteVertexArrays(m_sizes.size(), m_vao);
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
    return m_vao[m_textureId];
}

GLuint Sprite::getIdx() const noexcept
{
    return m_textureId;
}

glm::vec3 Sprite::getCurrentClip() const noexcept
{
    return m_sizes[m_textureId];
}

void Sprite::setIdx(GLuint idx)
{
    assert(idx < m_sizes.size());
    m_textureId = idx;
}

GLuint Sprite::getWidth() const noexcept
{
    return m_sizes[m_textureId].x;
}

GLuint Sprite::getHeight() const noexcept
{
    return m_sizes[m_textureId].y;
}

GLuint Sprite::getDepth() const noexcept
{
    return m_sizes[m_textureId].z;
}

GLuint Sprite::getSpritesCount() const noexcept
{
    return m_sizes.size();
}

GLuint Sprite::getTextureID() const
{
    return m_textureIds[m_textureId];
}

