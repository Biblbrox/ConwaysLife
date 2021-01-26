#include "render/render.hpp"
#include "utils/math.hpp"

using glm::vec2;
using glm::mat4;
using glm::vec3;
using utils::math::rotate_around;
using utils::math::operator/;

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
                   const glm::vec3& pos, GLfloat angle)
{
    assert(texture.getVAO() != 0);

    const GLfloat half = texture.getWidth() / 2.f; // TODO: fix size
    const GLfloat centerX = pos.x + half;
    const GLfloat centerY = pos.y + half;
    const GLfloat centerZ = pos.z + half;

    const glm::vec3 scale = glm::vec3(texture.getWidth(), texture.getHeight(),
                                    texture.getDepth());

    mat4 rotation = rotate_around(mat4(1.f), vec3(centerX, centerY, centerZ), angle);
    mat4 translation = translate(mat4(1.f), vec3(pos.x, pos.y, pos.z));
    mat4 scaling = glm::scale(mat4(1.f), scale);
    program.leftMultModel(scaling * rotation * translation);
    program.updateModel();

    glBindTexture(GL_TEXTURE_2D, texture.getTextureID());
    glBindVertexArray(texture.getVAO());
    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
    glDrawArrays(GL_TRIANGLES, 0, 180);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);

    translation[3] = glm::vec4(-pos.x,  -pos.y, -pos.z, 1);
    rotation = rotate_around(mat4(1.f), vec3(centerX, centerY, centerZ), -angle);
    scaling = glm::scale(mat4(1.f), 1 / scale);
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
