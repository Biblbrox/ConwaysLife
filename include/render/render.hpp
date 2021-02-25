#ifndef RENDER_HPP
#define RENDER_HPP

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "shaderprogram.hpp"
#include "sprite.hpp"
#include "utils/math.hpp"

using glm::vec2;

namespace render
{
    using glm::vec2;
    using glm::mat4;
    using glm::vec3;
    using utils::math::rotate_around;
    using utils::math::operator/;

    /**
     * Render some subclass of Texture.
     * @param program
     * @param texture
     * @param x
     * @param y
     * @param angle
     * @param scale_factor
     */
    void drawTextureScale(ShaderProgram& program, const Texture &texture,
                          const glm::vec3& pos, GLfloat angle);

    void drawTextureScale(ShaderProgram& program, const Texture &texture,
                          const glm::vec3& pos);

    void inline drawTexture(ShaderProgram& program, const Texture &texture,
                             const glm::vec3& position)
    {
        assert(texture.getVAO() != 0);

        glm::vec3 pos = 2.f * position;

        mat4& model = program.getModel();
        model[3] += glm::vec4(pos.x,  pos.y, pos.z, 0);

        program.updateModel();

//        glDrawArrays(GL_TRIANGLES, 0, 105);
        glDrawElements(GL_TRIANGLES, 333, GL_UNSIGNED_INT, nullptr);

        model[3] += glm::vec4(-pos.x,  -pos.y, -pos.z, 0);

        program.updateModel();
    }
    
    void drawLinen(const std::vector<vec2>& points, bool adjacency = false);

    void drawDots(const std::vector<vec2>& dots);

    void drawTriangles(const std::vector<vec2> &points);
};

#endif //RENDER_HPP
