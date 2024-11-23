#ifndef NCM_VERTEX_HPP
#define NCM_VERTEX_HPP

typedef struct vertex
{
    float x, y;
} vertex_t;

bool operator==(vertex_t const& lhs, vertex_t const& rhs);
vertex_t operator+(vertex_t const& lhs, vertex_t const& rhs);
vertex_t operator-(vertex_t const& lhs, vertex_t const& rhs);

vertex_t operator*(vertex_t const& v, float f);
vertex_t operator/(vertex_t const& v, float f);

int orientation(vertex_t const& a, vertex_t const& b, vertex_t const& c);
bool intersect(vertex_t const& u1, vertex_t const& u2, vertex_t const& v1, vertex_t const& v2);
float dot(vertex_t const& u1, vertex_t const& u2);
float distance(vertex_t const& u1, vertex_t const& u2);

#endif // NCM_VERTEX_HPP
