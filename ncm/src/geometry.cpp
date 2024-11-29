#include "geometry.hpp"

#include <cmath>
#include <algorithm>

// Very coarse implementations
// c.f for robust/exact solutions https://github.com/wlenthe/GeometricPredicates
bool on_segment(vertex_t const& u1, vertex_t const& u2, vertex_t const& v)
{
    return (v.x <= std::max(u1.x, u2.x) && v.x >= std::min(u1.x, u2.x) &&
            v.y <= std::max(u1.y, u2.y) && v.y >= std::min(u1.y, u2.y));
}

int orientation(vertex_t const& a, vertex_t const& b, vertex_t const& p)
{
    // -1: CW, 0: Collinear, 1: CCW
    float det = (a.x - p.x)*(b.y - p.y) - (b.x - p.x)*(a.y - p.y);
    if (std::abs(det) < std::numeric_limits<float>::epsilon())
        return 0;
    else
        return det < 0.f ? -1 : 1;
}

bool is_in_circle(vertex_t const& a, vertex_t const& b, vertex_t const& c, vertex_t const& p)
{
    float det = (a.x*a.x + a.y*a.y)*((b.x - p.x)*(c.y - p.y) - (c.x - p.x)*(b.y - p.y)) +
                (b.x*b.x + b.y*b.y)*((c.x - p.x)*(a.y - p.y) - (a.x - p.x)*(c.y - p.y)) +
                (c.x*c.x + c.y*c.y)*((a.x - p.x)*(b.y - p.y) - (b.x - p.x)*(a.y - p.y));

    return det > 0.f;
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

float dot(vertex_t const& u1, vertex_t const& u2)
{
    return u1.x*u2.x + u1.y*u2.y;
}

float distance(vertex_t const& u1, vertex_t const& u2)
{
    return std::sqrt(dot(u1+u2, u1+u2));
}

