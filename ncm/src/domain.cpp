#include "domain.hpp"

#include <fstream>
#include <sstream>
#include <tuple>
#include "parse.hpp"

multiple_definitions_error::multiple_definitions_error(std::string const& filename, std::string const& name)
    : error("Multiple definitions of object " + name + " in file " + filename) {}

std::tuple<std::string, vertex_t> read_vertex(std::stringstream& ss, file_location_t const& loc)
{
    std::string name;
    float x, y;
    ss >> name >> x >> y;

    if (!ss)
        throw invalid_file_format(loc);

    return {name, {.x = x, .y = y}};
}

std::tuple<std::string, boundary_t> read_boundary(std::stringstream& ss, file_location_t const& loc)
{
    std::string name, v1, v2, type_str;
    boundary_type type;
    ss >> name >> v1 >> v2 >> type_str;

    if (!ss)
        throw invalid_file_format(loc);

    if (type_str == "FREE")
        type = boundary_type::FREE;
    else if (type_str == "FIXED")
        type = boundary_type::FIXED;
    else if (type_str == "USER")
        type = boundary_type::USER;
    else
        throw invalid_file_expression(loc);

    return {name, {.v1 = v1, .v2 = v2, .type = type}};
}

domain_t read_domain_file(std::string const& filename)
{
    domain_t domain = {};
    std::ifstream file(filename);

    if (!file.is_open())
        throw std::runtime_error("Error while opening file: " + filename);

    std::string line;
    size_t line_nr = 0;
    while (++line_nr, std::getline(file, line))
    {
        line = trim_comment(line);
        line = trim(line);
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string key;
        ss >> key;
        if (!ss)
            throw invalid_file_format({filename, line_nr, line});

        if (key == "VERTEX")
        {
            auto [name, vertex] = read_vertex(ss, {filename, line_nr, line});
            if (domain.vertices.find(name) != domain.vertices.end())
                throw multiple_definitions_error(filename, name);

            domain.vertices.emplace(name, vertex);
        }
        else if (key == "BOUNDARY")
        {
            auto [name, boundary] = read_boundary(ss, {filename, line_nr, line});
            if (domain.boundaries.find(name) != domain.boundaries.end())
                throw multiple_definitions_error(filename, name);

            domain.boundaries.emplace(name, boundary);
        }
        else
            throw invalid_file_expression({filename, line_nr, key});
    }

    return domain;
}

