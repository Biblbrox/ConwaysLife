#ifndef COLLISION_HPP
#define COLLISION_HPP

#include <cmath>
#include <glm/gtc/constants.hpp>
#include <GL/glew.h>

namespace coll
{
    /**
     * Build rectangle from rect with corrected coords by
     * rotation angle
     * @param rect
     * @param alpha
     * @return
     */
    inline utils::RectPoints
    buildRectPoints(const utils::Rect &rect, GLfloat alpha) noexcept
    {
        GLfloat bx, by, cx, cy, dx, dy;
        const GLfloat x = rect.x;
        const GLfloat y = rect.y;
        const GLfloat width = rect.w;
        const GLfloat height = rect.h;
        // angle must be in range 0 - 2*pi
        alpha -= glm::two_pi<GLfloat>()
                 * std::floor(alpha / glm::two_pi<GLfloat>());

        alpha = -alpha;

        GLfloat alpha_cos = std::cos(alpha);
        GLfloat alpha_sin = std::sin(alpha);

        bx = x + width * alpha_cos;
        by = y - width * alpha_sin;

        dx = x + height * alpha_sin;
        dy = y + height * alpha_cos;

        cx = bx + height * alpha_sin;
        cy = by + height * alpha_cos;

        return {{x,  y}, {bx, by}, {cx, cy}, {dx, dy}};
    }

    /**
     * Check whether first line (p11, p12) intersect with second (p21, p22)
     * @param p11
     * @param p12
     * @param p21
     * @param p22
     * @return
     */
    constexpr bool lineLine(const vec2 &p11, const vec2 &p12,
                            const vec2 &p21, const vec2 &p22) noexcept
    {
        GLfloat x1 = p11.x;
        GLfloat x2 = p12.x;
        GLfloat x3 = p21.x;
        GLfloat x4 = p22.x;

        GLfloat y1 = p11.y;
        GLfloat y2 = p12.y;
        GLfloat y3 = p21.y;
        GLfloat y4 = p22.y;

        // calculate the distance to intersection point
        GLfloat uA = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3))
                     / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));
        GLfloat uB = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3))
                     / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));

        // if uA and uB are between 0-1, lines are colliding
        return uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1;
    }
}

#endif //COLLISION_HPP
