#include "debug.hpp"

#include <iostream>
#include "config.hpp"
#include "domain.hpp"

void print_config(config_t const& cfg)
{
    std::cout << "output_file = " << cfg.output_file << std::endl
              << "domain_file = " << cfg.domain_file << std::endl
              << "E           = " << cfg.young_modulus << std::endl
              << "nu          = " << cfg.poisson_ratio << std::endl
              << "dens        = " << cfg.density << std::endl
              << "gravity     = " << cfg.gravity << std::endl
              << "resolution  = " << cfg.resolution << std::endl;
}

void print_domain(domain_t const& domain)
{
    for (auto vertex : domain.vertices)
        std::cout << "VERTEX " << vertex.first << " " << vertex.second.x << " " << vertex.second.y << std::endl;

    for (auto boundary : domain.boundaries)
    {
        std::string type_str = boundary.second.type == boundary_type::FREE ? "FREE" :
                               boundary.second.type == boundary_type::FIXED ? "FIXED" :
                               boundary.second.type == boundary_type::USER ? "USER" : "UNKNOWN";
        std::cout << "BOUNDARY " << boundary.first << " " << boundary.second.v1 << " " << boundary.second.v2 << " " << type_str << std::endl;
    }
} 

