#include "domain.hpp"

#include <fstream>
#include <tuple>
#include <cstdio>
#include <cassert>
#include "parse.hpp"

multiple_definitions_error::multiple_definitions_error(file_location_t const& loc)
    : error("Multiple definitions of object " + loc.expr + " in file " + loc.file + " @ line " + std::to_string(loc.line)) {}

std::tuple<std::string, vertex_t> read_vertex(std::string_view line, file_location_t const& loc)
{
    char name[32];
    float x, y;
    int read = std::sscanf(line.data(), "%31s %f %f", name, &x, &y);

    if (read < 3)
        throw invalid_file_format(loc);

    return {name, {.x = x, .y = y}};
}

std::tuple<std::string, boundary_t> read_boundary(std::string_view line, file_location_t const& loc)
{
    char name[32], v1[32], v2[32], type_str[32];
    boundary_type type;
    int read = std::sscanf(line.data(), "%31s %31s %31s %31s", name, v1, v2, type_str);

    if (read < 4)
        throw invalid_file_format(loc);

    if (!std::strcmp(type_str, "FREE"))
        type = boundary_type::FREE;
    else if (!std::strcmp(type_str, "FIXED"))
        type = boundary_type::FIXED;
    else if (!std::strcmp(type_str, "USER"))
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

        char key[16];
        int pos = 0;
        int read = std::sscanf(line.c_str(), "%15s%n", key, &pos);
        if (!read)
            throw invalid_file_format({filename, line_nr, line});

        if (!std::strcmp(key, "VERTEX"))
        {
            auto [name, vertex] = read_vertex(&line.data()[pos+1], {filename, line_nr, line});
            if (domain.vertices.find(name) != domain.vertices.end())
                throw multiple_definitions_error({filename, line_nr, name});

            domain.vertices.emplace(name, vertex);
        }
        else if (!std::strcmp(key, "BOUNDARY"))
        {
            auto [name, boundary] = read_boundary(&line.data()[pos+1], {filename, line_nr, line});
            if (domain.boundaries.find(name) != domain.boundaries.end())
                throw multiple_definitions_error({filename, line_nr, name});

            domain.boundaries.emplace(name, boundary);
        }
        else
            throw invalid_file_expression({filename, line_nr, key});
    }

    return domain;
}

void validate_domain(domain_t const& domain)
{

}

