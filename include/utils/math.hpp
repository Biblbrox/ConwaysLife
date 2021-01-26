#ifndef MATH_HPP
#define MATH_HPP

#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

using glm::vec2;

namespace utils::math {

    constexpr unsigned int power_two(unsigned int val) noexcept
    {
        unsigned int power = 2, nextVal = power * 2;


        while ((nextVal *= 2) <= val)
            power *= 2;

        return power * 2;
    }

    /**
     * Rotate(move elements) around element with idx position
     * @param points
     * @param idx
     * @return
     */
    inline std::vector<vec2> rotate_points(const std::vector<vec2>& points, size_t idx)
    {
        assert(idx < points.size());

        std::vector<vec2> res;
        res.reserve(points.size());
        for (size_t i = 0; i < points.size(); ++i)
            res.emplace_back(points[i] + 2.f * (points[idx] - points[i]));

        std::reverse(res.begin(), res.end());
        res.shrink_to_fit();

        return res;
    }

    /**
     * Rotate world around point v
     * @param m
     * @param v
     * @param angle
     * @return
     */
    inline glm::mat4
    rotate_around(const glm::mat4 &m, const glm::vec3 &v, GLfloat angle)
    {
        glm::mat4 tr1 = glm::translate(m, v);
        glm::mat4 ro = glm::rotate(tr1, angle, glm::vec3(0.f, 0.f, 1.f));
        glm::mat4 tr2 = glm::translate(ro, -v);

        return tr2;
    }

    template <typename T>
    inline glm::vec3 operator/(T val, const glm::vec3& vec)
    {
        static_assert(std::is_arithmetic_v<T>, "Template parameter"
                                                   " must be arithmetical");
        return glm::vec3(val / vec.x, val / vec.y, val / vec.z);
    }

}

#endif //MATH_HPP
