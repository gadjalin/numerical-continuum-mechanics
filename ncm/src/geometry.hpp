#ifndef NCM_GEOMETRY_HPP
#define NCM_GEOMETRY_HPP

#include "vertex.hpp"

int orientation(vertex_t const& a, vertex_t const& b, vertex_t const& p);
bool is_in_circle(vertex_t const& a, vertex_t const& b, vertex_t const& c, vertex_t const& p);
bool intersect(vertex_t const& u1, vertex_t const& u2, vertex_t const& v1, vertex_t const& v2);
float dot(vertex_t const& u1, vertex_t const& u2);
float distance(vertex_t const& u1, vertex_t const& u2);

#endif // NCM_GEOMETRY_HPP
