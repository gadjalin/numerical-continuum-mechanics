#ifndef NCM_VERTEX_HPP
#define NCM_VERTEX_HPP

typedef struct vertex
{
    float x, y;
} vertex_t;

bool operator==(vertex_t const& lhs, vertex_t const& rhs);
bool operator!=(vertex_t const& lhs, vertex_t const& rhs);

void operator+=(vertex_t& lhs, vertex_t const& rhs);
void operator-=(vertex_t& lhs, vertex_t const& rhs);
void operator*=(vertex_t& v, float f);
void operator/=(vertex_t& v, float f);

vertex_t operator+(vertex_t const& lhs, vertex_t const& rhs);
vertex_t operator-(vertex_t const& lhs, vertex_t const& rhs);
vertex_t operator*(vertex_t const& v, float f);
vertex_t operator/(vertex_t const& v, float f);

#endif // NCM_VERTEX_HPP
