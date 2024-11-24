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
    std::array<size_t, 2> nodes;
} edge_t;

typedef struct element
{
    std::array<size_t, 3> nodes;
} element_t;

typedef struct mesh
{
    std::vector<vertex_t> nodes;
    std::vector<edge_t> edges;
    std::vector<element_t> elements;
    std::map<size_t, size_t> nearest_neighbours;
} mesh_t;

mesh_t generate_mesh(domain_t const& domain, float resolution);
void save_mesh(mesh const& mesh, std::string const& filename);

#endif // NCM_MESH_HPP
