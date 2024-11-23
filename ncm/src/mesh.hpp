#ifndef NCM_MESH_HPP
#define NCM_MESH_HPP

#include <array>
#include <vector>

#include "vertex.hpp"
#include "domain.hpp"

typedef struct box
{
    vertex_t lower;
    vertex_t upper;
} box_t;

typedef struct edge
{
    std::array<unsigned int, 2> nodes;
} edge_t;

typedef struct element
{
    std::array<unsigned int, 3> nodes;
} element_t;

typedef struct mesh
{
    std::vector<vertex_t> nodes;
    std::vector<edge_t> edges;
    std::vector<element_t> elements;
} mesh_t;

mesh_t generate_mesh(domain_t const& domain, float resolution);

#endif // NCM_MESH_HPP
