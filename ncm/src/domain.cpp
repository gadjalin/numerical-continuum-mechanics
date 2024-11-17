#include "domain.hpp"

#include <cstring>
#include <cstdio>
#include <cmath>
#include <cassert>

#include <map>
#include <vector>
#include <fstream>
#include <tuple>

#include <nail/Utils/String.hpp>

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

    template<typename Key, typename Value, typename UnaryFn>
    void compare_each(std::map<Key, Value> const& m, UnaryFn fn)
    {
        for (auto it = m.cbegin(); it != m.cend(); ++it)
            for (auto it2 = std::next(it); it2 != m.cend(); ++it2)
                fn(*it, *it2);
    }

    template<typename Key, typename Value, typename UnaryPred>
    bool compare_all_of(std::map<Key, Value> const& m, UnaryPred p)
    {
        return !compare_any_of(m, [&p](auto const& lhs, auto const& rhs) { return !p(lhs, rhs); });
    }

    template<typename Key, typename Value, typename UnaryPred>
    bool compare_any_of(std::map<Key, Value> const& m, UnaryPred p)
    {
        for (auto it = m.cbegin(); it != m.cend(); ++it)
            for (auto it2 = std::next(it); it2 != m.cend(); ++it2)
                if (p(*it, *it2))
                    return true;

        return false;
    }

    template<typename Key, typename Value, typename UnaryPred>
    bool compare_none_of(std::map<Key, Value> const& m, UnaryPred p)
    {
        return !compare_any_of(m, p);
    }

    std::string join_pairs(std::vector<std::pair<std::string, std::string> > const& name_map)
    {
        std::string line;
        for (auto const& [s1, s2] : name_map)
            line += "\t\t(" + s1 + "," + s2 + ")\n";
        return line;
    }

    std::string join_vertex_boundary(std::vector<std::pair<std::string, std::string> > const& name_map)
    {
        std::string line;
        for (auto const& [boundary, vertex] : name_map)
            line += "\t\tVertex " + vertex + " referenced in boundary " + boundary + "\n";
        return line;
    }

    std::vector<std::pair<std::string, std::string> > find_duplicate_vertices(std::map<std::string, vertex_t> const& vertices)
    {
        std::vector<std::pair<std::string, std::string>> duplicates;
        compare_each(vertices,
        [&duplicates](auto const& v1, auto const& v2) -> void
        {
            float dx = v1.second.x - v2.second.x;
            float dy = v1.second.y - v2.second.y;
            bool are_same = (std::sqrt(dx*dx + dy*dy)) < SMALL_DX;
            if (are_same)
                duplicates.push_back({v1.first, v2.first});
        });

        return duplicates;
    }

    std::vector<std::pair<std::string, std::string> > find_undefined_vertices(std::map<std::string, vertex_t> const& vertices,
                                                                              std::map<std::string, boundary_t> const& boundaries)
    {
        std::vector<std::pair<std::string, std::string> > undefined_vertices;
        for (auto const& [name, boundary] : boundaries)
        {
            if (vertices.find(boundary.v1) == vertices.end())
                undefined_vertices.push_back({name, boundary.v1});
            if (vertices.find(boundary.v2) == vertices.end())
                undefined_vertices.push_back({name, boundary.v2});
        }

        return undefined_vertices;
    }

    std::vector<std::pair<std::string, std::string> > find_duplicate_boundaries(std::map<std::string, boundary_t> const& boundaries)
    {
        std::vector<std::pair<std::string, std::string> > duplicates;
        compare_each(boundaries,
        [&duplicates](auto const& b1, auto const& b2) -> void
        {
            bool are_same = (b1.second.v1 == b2.second.v1 && b1.second.v2 == b2.second.v2) ||
                            (b1.second.v1 == b2.second.v2 && b1.second.v2 == b2.second.v1);
            if (are_same)
                duplicates.push_back({b1.first, b2.first});
        });

        return duplicates;
    }

    std::vector<std::string> find_zero_length_boundaries(std::map<std::string, boundary_t> const& boundaries)
    {
        std::vector<std::string> ids;
        for (auto const& [name, boundary] : boundaries)
            if (boundary.v1 == boundary.v2)
                ids.push_back(name);

        return ids;
    }

    std::vector<std::pair<std::string, std::string> > find_boundaries_intersect(std::map<std::string, vertex_t> const& vertices,
                                                                                std::map<std::string, boundary_t> const& boundaries)
    {
        std::vector<std::pair<std::string, std::string> > intersects;
        compare_each(boundaries,
        [&intersects, &vertices](auto const& b1, auto const& b2) -> void
        {
            vertex_t u1 = vertices.at(b1.second.v1);
            vertex_t u2 = vertices.at(b1.second.v2);
            vertex_t v1 = vertices.at(b2.second.v1);
            vertex_t v2 = vertices.at(b2.second.v2);
            bool do_intersect = intersect(u1, u2, v1, v2);
            if (do_intersect)
                intersects.push_back({b1.first, b2.first});
        });

        return intersects;
    }

    std::vector<std::pair<std::string, std::string> > find_dangling_vertices(std::map<std::string, boundary_t> const& boundaries)
    {
        std::map<std::string, std::vector<std::string>> connections;
        std::vector<std::pair<std::string, std::string> > danglings;

        for (auto [key, boundary] : boundaries)
        {
            connections[boundary.v1].push_back(key);
            connections[boundary.v2].push_back(key);
        }

        for (auto [key, links] : connections)
            if (links.size() == 1)
                danglings.push_back({links.front(), key});

        return danglings;
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

    auto name_map = find_undefined_vertices(domain.vertices, domain.boundaries);
    if (!name_map.empty())
        throw std::logic_error("Undefined vertices: \n" + join_vertex_boundary(name_map));

    name_map = find_duplicate_vertices(domain.vertices);
    if (!name_map.empty())
        throw std::logic_error("Vertices have similar coordinates: \n" + join_pairs(name_map));

    name_map = find_duplicate_boundaries(domain.boundaries);
    if (!name_map.empty())
        throw std::logic_error("Boundaries have same end points: \n" + join_pairs(name_map));

    auto name_list = find_zero_length_boundaries(domain.boundaries);
    if (!name_list.empty())
        throw std::logic_error("Boundaries have zero-length: " + nail::join_all(name_list.cbegin(), name_list.cend(), ", "));
                               

    name_map = find_boundaries_intersect(domain.vertices, domain.boundaries);
    if (!name_map.empty())
        throw std::logic_error("Boundaries intersect: \n" + join_pairs(name_map));
    // Check that every boundary is connected to another on both ends
    // Check that all connected boundary form a closed shape (implicit from previous check?)
    name_map = find_dangling_vertices(domain.boundaries);
    if (!name_map.empty())
        throw std::logic_error("Dangling connection: \n" + join_vertex_boundary(name_map));
}

