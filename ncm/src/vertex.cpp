#include "vertex.hpp"

#include <cmath>

bool operator==(vertex_t const& lhs, vertex_t const& rhs)
{
    return (&lhs == &rhs) || (std::abs(lhs.x - rhs.x) <= std::numeric_limits<float>::epsilon() &&
                              std::abs(lhs.y - rhs.y) <= std::numeric_limits<float>::epsilon());
}

bool operator!=(vertex_t const& lhs, vertex_t const& rhs)
{
    return !operator==(lhs, rhs);
}

void operator+=(vertex_t& lhs, vertex_t const& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
}

void operator-=(vertex_t& lhs, vertex_t const& rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
}

void operator*=(vertex_t& v, float const f)
{
    v.x *= f;
    v.y *= f;
}

void operator/=(vertex_t& v, float const f)
{
    v.x /= f;
    v.y /= f;
}

vertex_t operator+(vertex_t const& lhs, vertex_t const& rhs)
{
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

vertex_t operator-(vertex_t const& lhs, vertex_t const& rhs)
{
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

vertex_t operator*(vertex_t const& v, float const f)
{
    return {v.x * f, v.y * f};
}

vertex_t operator/(vertex_t const& v, float const f)
{
    return {v.x / f, v.y / f};
}

