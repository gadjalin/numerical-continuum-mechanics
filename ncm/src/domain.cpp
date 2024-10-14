#include "domain.hpp"

#include <fstream>
#include <sstream>
#include "parse.hpp"

multiple_definitions::multiple_definitions(std::string const& filename, std::string const& name)
    : std::runtime_error("Multiple definitions of object " + name + " in file " + filename) {}

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
            throw invalid_file_format(filename, line_nr, line);

        if (key == "VERTEX")
        {
            std::string name;
            float x, y;
            ss >> name >> x >> y;
            if (!ss)
                throw invalid_file_format(filename, line_nr, line);
            else if (domain.vertices.find(name) != domain.vertices.end())
                throw multiple_definitions(filename, name);

            domain.vertices.emplace(name, vertex_t{.x = x, .y = y});
        }
        else if (key == "BOUNDARY")
        {
            std::string name, v1, v2, type_str;
            boundary_type type;
            ss >> name >> v1 >> v2 >> type_str;
            if (!ss)
                throw invalid_file_format(filename, line_nr, line);
            else if (domain.boundaries.find(name) != domain.boundaries.end())
                throw multiple_definitions(filename, name);

            if (type_str == "FREE")
                type = boundary_type::FREE;
            else if (type_str == "FIXED")
                type = boundary_type::FIXED;
            else if (type_str == "USER")
                type = boundary_type::USER;
            else
                throw unknown_file_key(filename, line_nr, type_str);

            domain.boundaries.emplace(name, boundary_t{.v1 = v1, .v2 = v2, .type = type});
        }
        else
            throw unknown_file_key(filename, line_nr, key);
    }


    return domain;
}

