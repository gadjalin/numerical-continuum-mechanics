#include "config.hpp"

#include <fstream>
#include "parse.hpp"

config_t read_config_file(std::string const& filename)
{
    config_t cfg = {};

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

        std::size_t equal_sign_pos = line.find('=');
        if (equal_sign_pos == std::string::npos)
            throw invalid_file_format({filename, line_nr, line});

        std::string key = trim(line.substr(0, equal_sign_pos));
        std::string value = trim(line.substr(equal_sign_pos + 1));
        // Remove quotes if value contains a string
        if (value.starts_with("\"")) value.erase(0, 1);
        if (value.ends_with("\"")) value.erase(value.size() - 1);

        if (key == "output_file")
            cfg.output_file = value;
        else if (key == "domain_file")
            cfg.domain_file = value;
        else if (key == "E")
            cfg.young_modulus = std::stof(value);
        else if (key == "nu")
            cfg.poisson_ratio = std::stof(value);
        else if (key == "dens")
            cfg.density = std::stof(value);
        else if (key == "g")
            cfg.gravity = std::stof(value);
        else if (key == "resolution")
            cfg.resolution = std::stof(value);
        else
            throw invalid_file_expression({filename, line_nr, key});
    }

    return cfg;
}

void check_config(config_t const& cfg)
{
    if (cfg.output_file.empty()) throw std::runtime_error("Empty output file name!");
    if (cfg.domain_file.empty()) throw std::runtime_error("Empty mesh file name!");
}

