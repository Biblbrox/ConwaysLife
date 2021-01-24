#include <numeric>
#include <iostream>

#include "systems/collisionsystem.hpp"
#include "utils/collision.hpp"

using glm::vec2;

void CollisionSystem::update_state(size_t delta)
{

}

/**
 * Find lines that affected by any of points of rectangle rect
 * @param rect
 * @param points
 * @return
 */
std::vector<size_t> find_lines_under(const utils::RectPoints& rect,
                                     const std::vector<vec2> &points)
{
    GLfloat left_most = std::min({rect.a.x, rect.b.x, rect.c.x, rect.d.x});
    size_t line_idx = std::prev(
            std::lower_bound(points.cbegin(), points.cend(), left_most,
                             [](const vec2& point, GLfloat val) {
                                 return point.x < val;
                             })) - points.cbegin();

    // Assume that ship can't be place at more than 4 lines simultaneously
    std::vector<size_t> res(4);
    std::iota(res.begin(), res.end(), line_idx);

    return res;
}


bool
CollisionSystem::levelSpriteCollision(const Sprite &sprite, GLfloat ship_x,
                                      GLfloat ship_y, const std::vector<vec2> &points,
                                      const std::vector<vec2> &stars, GLfloat angle)
{
    utils::Rect coords = sprite.getCurrentClip();
    coords.x = ship_x;
    coords.y = ship_y;
    utils::RectPoints r = coll::buildRectPoints(coords, angle);

    std::vector<size_t> lines = find_lines_under(r, points);

    return std::any_of(lines.cbegin(), lines.cend(), [points, r](size_t idx) {
        vec2 p = points[idx];
        vec2 p_right = points[idx + 1];
        bool left = coll::lineLine(r.d, r.a, {p.x, p.y}, {p_right.x, p_right.y});
        bool right = coll::lineLine(r.b, r.c, {p.x, p.y}, {p_right.x, p_right.y});
        bool top = coll::lineLine(r.c, r.d, {p.x, p.y}, {p_right.x, p_right.y});
        bool bottom = coll::lineLine(r.a, r.b, {p.x, p.y}, {p_right.x, p_right.y});

        return left || right || top || bottom;
    });
}