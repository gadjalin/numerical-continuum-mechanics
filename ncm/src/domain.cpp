#include "domain.hpp"

#include <cstring>
#include <cstdio>
#include <cmath>

#include <map>
#include <vector>
#include <fstream>
#include <tuple>
#include <algorithm>

multiple_definitions_error::multiple_definitions_error(file_location_t const& loc)
    : error("Multiple definitions of object " + loc.expr + " in file " + loc.file + " @ line " + std::to_string(loc.line)) {}

namespace
{
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

    template<typename Key, typename Value, typename Predicate>
    bool compare_any_of(std::map<Key, Value> const& m, Predicate p)
    {
        for (auto it = m.cbegin(); it != m.cend(); ++it)
            for (auto it2 = std::next(it); it2 != m.cend(); ++it2)
                if (p(*it, *it2))
                    return true;

        return false;
    }

    void check_duplicate_vertices(std::map<std::string, vertex_t> const& vertices)
    {
        std::string k1, k2;
        bool has_duplicate = compare_any_of(vertices,
        [&k1, &k2](auto const& v1, auto const& v2) -> bool
        {
            float dx = v1.second.x - v2.second.x;
            float dy = v1.second.y - v2.second.y;
            bool are_same = (std::sqrt(dx*dx + dy*dy)) < SMALL_DX;
            if (are_same)
            {
                k1 = v1.first;
                k2 = v2.first;
            }

            return are_same;
        });

        if (has_duplicate)
            throw std::logic_error("Vertices have similar coordinates: " + k1 +  "," + k2);
    }

    void check_boundaries_valid(std::map<std::string, vertex_t> const& vertices,
                                std::map<std::string, boundary_t> const& boundaries)
    {
        for (auto const& [name, boundary] : boundaries)
        {
            if (vertices.find(boundary.v1) == vertices.end())
                throw std::logic_error("Undefined vertex " + boundary.v1 + " for boundary " + name + "!");
            if (vertices.find(boundary.v2) == vertices.end())
                throw std::logic_error("Undefined vertex " + boundary.v2 + " for boundary " + name + "!");
        }
    }

    void check_duplicate_boundaries(std::map<std::string, boundary_t> const& boundaries)
    {
        std::string k1, k2;
        bool has_duplicate = compare_any_of(boundaries,
        [&k1, &k2](auto const& b1, auto const& b2) -> bool
        {
            bool are_same = (b1.second.v1 == b2.second.v1 && b1.second.v2 == b2.second.v2) ||
                            (b1.second.v1 == b2.second.v2 && b1.second.v2 == b2.second.v1);
            if (are_same)
            {
                k1 = b1.first;
                k2 = b2.first;
            }

            return are_same;
        });

        if (has_duplicate)
            throw std::logic_error("Boundaries have same end points: " + k1 + "," + k2);
    }

    void check_zero_length_boundary(std::map<std::string, boundary_t> const& boundaries)
    {
        std::string k;
        bool has_null_boundary = std::any_of(boundaries.begin(), boundaries.end(),
        [&k](auto const& b) -> bool
        {
            bool is_null = b.second.v1 == b.second.v2;
            if (is_null)
                k = b.first;

            return is_null;
        });

        if (has_null_boundary)
            throw std::logic_error("Boundary has zero-length: " + k);
    }

    void check_boundaries_intersect(std::map<std::string, vertex_t> const& vertices,
                                    std::map<std::string, boundary_t> const& boundaries)
    {
        std::string k1, k2;
        bool has_intersect = compare_any_of(boundaries,
        [&k1,&k2, &vertices](auto const& b1, auto const& b2) -> bool
        {
            vertex_t u1 = vertices.at(b1.second.v1);
            vertex_t u2 = vertices.at(b1.second.v2);
            vertex_t v1 = vertices.at(b2.second.v1);
            vertex_t v2 = vertices.at(b2.second.v2);
            bool do_intersect = intersect(u1, u2, v1, v2);
            if (do_intersect)
            {
                k1 = b1.first;
                k2 = b2.first;
            }

            return do_intersect;
        });

        if (has_intersect)
            throw std::logic_error("Boundaries intersect: " + k1 + ", " + k2);
    }

    void check_boundaries_are_closed(std::map<std::string, boundary_t> const& boundaries)
    {
        std::map<std::string, std::vector<std::string>> connections;

        for (auto [key, boundary] : boundaries)
        {
            connections[boundary.v1].push_back(key);
            connections[boundary.v2].push_back(key);
        }

        for (auto [key, links] : connections)
            if (links.size() == 1)
                throw std::logic_error("Dangling connection: vertex " + key + " on boundary " + links.front());
    }
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
    // Basic logic checks
    if (domain.vertices.size() < 3) throw std::logic_error("Too few vertice!");
    if (domain.boundaries.size() < 3) throw std::logic_error("Too few boundaries!");

    check_boundaries_valid(domain.vertices, domain.boundaries);

    check_duplicate_vertices(domain.vertices);
    check_duplicate_boundaries(domain.boundaries);
    check_zero_length_boundary(domain.boundaries);
    check_boundaries_intersect(domain.vertices, domain.boundaries);
    // Check that every boundary is connected to another on both ends
    // Check that all connected boundary form a closed shape (implicit from previous check?)
    check_boundaries_are_closed(domain.boundaries);
}

