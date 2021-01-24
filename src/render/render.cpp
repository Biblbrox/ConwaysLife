#include "render/render.hpp"
#include "utils/math.hpp"

using glm::vec2;
using glm::mat4;
using glm::vec3;
using utils::math::rotate_around;

void render::drawLinen(const std::vector<vec2>& points, bool adjacency)
{
    if (adjacency)
        assert(points.size() % 2 == 0);

    auto vertices = points.data();

    GLuint verticesID = 0;
    GLuint VAO = 0;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &verticesID);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, verticesID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * points.size(), vertices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glDrawArrays(adjacency ? GL_LINE_STRIP_ADJACENCY : GL_LINES, 0, points.size());

    glDisableVertexAttribArray(0);

    glDeleteBuffers(1, &verticesID);
    glDeleteVertexArrays(1, &VAO);
}

void render::drawDots(const std::vector<vec2>& dots)
{
    auto vertices = dots.data();

    GLuint VAO = 0;
    GLuint verticesID = 0;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &verticesID);
    glBindBuffer(GL_ARRAY_BUFFER, verticesID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * dots.size(), vertices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_POINTS, 0, dots.size());
    glDisableVertexAttribArray(0);

    glDeleteBuffers(1, &verticesID);
    glDeleteVertexArrays(1, &VAO);
}

void
render::drawTexture(ShaderProgram& program, const Texture &texture,
                   GLfloat x, GLfloat y, GLfloat angle, GLfloat scale_factor)
{
    assert(texture.getVAO() != 0);

    const GLfloat half = 0.5f;
    const GLfloat centerX = x + half;
    const GLfloat centerY = y + half;
    const GLfloat invScale = 1.f / scale_factor;

    mat4 rotation = rotate_around(mat4(1.f), vec3(centerX, centerY, 0.f), angle);
    mat4 translation = translate(mat4(1.f), vec3(x, y, 0.f));
    mat4 scaling = scale(mat4(1.f), vec3(scale_factor, scale_factor, 1.f));
    program.leftMultModel(scaling * rotation * translation);
    program.updateModel();

    glBindTexture(GL_TEXTURE_2D, texture.getTextureID());
    glBindVertexArray(texture.getVAO());
    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);

    translation[3] = glm::vec4(-x,  -y, 0.f, 1);
    rotation = rotate_around(mat4(1.f), vec3(centerX, centerY, 0.f), -angle);
    scaling[0][0] = invScale;
    scaling[1][1] = invScale;
    scaling[2][2] = invScale;
    program.leftMultModel(translation * rotation * scaling);
    program.updateModel();
}

void render::drawTriangles(const std::vector<vec2>& points)
{
    assert(points.size() % 3 == 0);
    auto vertices = points.data();

    GLuint verticesID = 0;
    GLuint VAO = 0;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &verticesID);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, verticesID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * points.size(), vertices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLES, 0, points.size());
    glDisableVertexAttribArray(0);

    glDeleteBuffers(1, &verticesID);
    glDeleteVertexArrays(1, &VAO);
}
