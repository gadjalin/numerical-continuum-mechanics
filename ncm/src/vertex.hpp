#ifndef NCM_VERTEX_HPP
#define NCM_VERTEX_HPP

typedef struct vertex
{
    float x, y;
} vertex_t;

bool operator==(vertex_t const& lhs, vertex_t const& rhs);
bool intersect(vertex_t const& u1, vertex_t const& u2, vertex_t const& v1, vertex_t const& v2);

#endif // NCM_VERTEX_HPP
