#include "mesh.hpp"

namespace
{
    box_t get_domain_bounds(domain_t const& domain)
    {
        box_t bounds = {domain.vertices.begin()->second, domain.vertices.begin()->second};
        for (auto const [name, vertex] : domain.vertices)
        {
            bounds.lower.x = std::min(bounds.lower.x, vertex.x);
            bounds.lower.y = std::min(bounds.lower.y, vertex.y);
            bounds.upper.x = std::max(bounds.upper.x, vertex.x);
            bounds.upper.y = std::max(bounds.upper.y, vertex.y);
        }

        return bounds;
    }

    std::vector<vertex_t> generate_uniform_grid(box_t const box, float const resolution)
    {
        std::vector<vertex_t> grid;

        size_t num_node_x = (box.upper.x - box.lower.x) / resolution + 1;
        size_t num_node_y = (box.upper.y - box.lower.y) / resolution + 1;
        grid.reserve(num_node_x * num_node_y);
        for (int i = 0; i < num_node_x; i++)
            for (int j = 0; j < num_node_y; j++)
                grid.push_back({i * resolution, j * resolution});

        return grid;
    }

    void insert_refined_edges(mesh_t& mesh, domain_t const& domain, float const resolution)
    {
        for (auto const& [name, boundary] : domain.boundaries)
        {
            vertex_t v1 = domain.vertices.at(boundary.v1);
            vertex_t v2 = domain.vertices.at(boundary.v2);
            vertex_t v1v2 = v1 - v2;
            int num_refinement = std::ceil(distance(v1, v2) / resolution);
            for (int i = 0; i < num_refinement; ++i)
                mesh.nodes.push_back(v2 + v1v2 * (float(i)/num_refinement));
        }
    }
}

mesh_t generate_mesh(domain_t const& domain, float resolution)
{
    mesh_t mesh;

    box_t bounds = get_domain_bounds(domain);
    mesh.nodes = generate_uniform_grid(bounds, resolution);
    insert_refined_edges(mesh, domain, resolution);

    return mesh;
}

