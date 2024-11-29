#include "mesh.hpp"

#include <cstdio>

#include "geometry.hpp"

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
            int num_steps = int(distance(v1, v2) / resolution);

            size_t offset = mesh.nodes.size();
            mesh.nodes.push_back(v1);
            mesh.nodes.push_back(v2);
            mesh.edges.push_back({offset, offset + 1});
            for (int i = 1; i < num_steps; ++i)
                mesh.nodes.push_back(v2 + v1v2 * (float(i)/num_steps));
        }
    }

    bool is_in_polygon(vertex_t const& v, std::vector<vertex_t> const& vertices, std::vector<edge_t> const& edges)
    {
        int crossings = 0;
        for (edge_t const& edge : edges)
        {
            vertex_t v1 = vertices.at(edge.nodes[0]);
            vertex_t v2 = vertices.at(edge.nodes[1]);
            if (v.y <= std::max(v1.y, v2.y) && v.y >= std::min(v1.y, v2.y))
            {
                if (v.x <= std::min(v1.x, v2.x))
                    crossings++;
                else if (v.x <= std::max(v1.x, v2.x))
                    if (orientation(v1.y < v2.y ? v1 : v2, v1.y < v2.y ? v2 : v1, v) < 0)
                        crossings++;
            }
        }
        return crossings % 2 == 1;
    }

    void remove_outside_nodes(mesh_t& mesh)
    {
    }

    void init_neighbours(mesh_t& mesh)
    {

    }
}

mesh_t generate_mesh(domain_t const& domain, float resolution)
{
    mesh_t mesh;

    box_t bounds = get_domain_bounds(domain);
    mesh.nodes = generate_uniform_grid(bounds, resolution);
    insert_refined_edges(mesh, domain, resolution);
    //remove_outside_nodes(mesh);
    //init_neighbours(mesh);

    return mesh;
}

void save_mesh(mesh const& mesh, std::string const& filename)
{
    FILE* mesh_file;
    mesh_file = std::fopen(filename.c_str(), "w");
    if (!mesh_file)
        throw std::runtime_error("Unable to save mesh");

    std::fprintf(mesh_file, "%8ld\n", mesh.nodes.size());
    for (vertex_t const& vertex : mesh.nodes)
        std::fprintf(mesh_file, "%12.5f %12.5f\n", vertex.x, vertex.y);

    std::fprintf(mesh_file, "%8ld\n", mesh.edges.size());
    for (edge_t const& edge : mesh.edges)
        std::fprintf(mesh_file, "%8ld %8ld\n", edge.nodes[0], edge.nodes[1]);

    std::fprintf(mesh_file, "%8ld\n", mesh.elements.size());
    for (element_t const& element : mesh.elements)
        std::fprintf(mesh_file, "%8ld %8ld %8ld\n", element.nodes[0], element.nodes[1], element.nodes[2]);

    std::fclose(mesh_file);
}

