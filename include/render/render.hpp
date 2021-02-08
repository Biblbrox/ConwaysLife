#ifndef RENDER_HPP
#define RENDER_HPP

#include <GL/glew.h>

#include "shaderprogram.hpp"
#include "sprite.hpp"

using glm::vec2;

namespace render
{
    /**
     * Render some subclass of Texture.
     * @param program
     * @param texture
     * @param x
     * @param y
     * @param angle
     * @param scale_factor
     */
    void drawTexture(ShaderProgram& program, const Texture &texture,
                     const glm::vec3& pos, GLfloat angle);

    void drawTexture(ShaderProgram& program, const Texture &texture,
                     const glm::vec3& pos);
    
    void drawLinen(const std::vector<vec2>& points, bool adjacency = false);

    void drawDots(const std::vector<vec2>& dots);

    void drawTriangles(const std::vector<vec2> &points);
};

#endif //RENDER_HPP
