#include "vertex.hpp"

#include <algorithm>

bool operator==(vertex_t const& lhs, vertex_t const& rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool on_segment(vertex_t const& u1, vertex_t const& u2, vertex_t const& v)
{
    return (v.x <= std::max(u1.x, u2.x) && v.x >= std::min(u1.x, u2.x) &&
            v.y <= std::max(u1.y, u2.y) && v.y >= std::min(u1.y, u2.y));
}

int orientation(vertex_t const& a, vertex_t const& b, vertex_t const& c)
{
    int det = (b.y - a.y) * (c.x - b.x) - (c.y - b.y) * (b.x - a.x);
    // collinear = 0, clock = 1, or counter clock wise = 2
    return det == 0 ? 0 : (det > 0 ? 1 : 2);
}

bool intersect(vertex_t const& u1, vertex_t const& u2, vertex_t const& v1, vertex_t const& v2)
{
    int o1 = orientation(u1, u2, v1);
    int o2 = orientation(u1, u2, v2);
    int o3 = orientation(v1, v2, u1);
    int o4 = orientation(v1, v2, u2);
    // If segments share a vertex
    if (u1 == v1 || u1 == v2 || u2 == v1 || u2 == v2)
        return false;
    else if (o1 != o2 && o3 != o4)
        return true;
    else if ((o1 == 0 && on_segment(u1, u2, v1)) ||
             (o2 == 0 && on_segment(u1, u2, v2)) ||
             (o3 == 0 && on_segment(v1, v2, u1)) ||
             (o4 == 0 && on_segment(v1, v2, u2)))
        return true;
    else
        return false;
}

