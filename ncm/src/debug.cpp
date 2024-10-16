#include "debug.hpp"

#include <iostream>
#include "config.hpp"
#include "domain.hpp"

void print_config(config_t const& cfg)
{
    std::cout << "###### Parameter file ######" << std::endl;
    std::cout << "\toutput_file = " << cfg.output_file << std::endl
              << "\tdomain_file = " << cfg.domain_file << std::endl
              << "\tE           = " << cfg.young_modulus << std::endl
              << "\tnu          = " << cfg.poisson_ratio << std::endl
              << "\tdens        = " << cfg.density << std::endl
              << "\tgravity     = " << cfg.gravity << std::endl
              << "\tresolution  = " << cfg.resolution << std::endl;
    std::cout << "############################" << std::endl;
}

void print_domain(domain_t const& domain)
{
    std::cout << "###### Domain file ######" << std::endl;
    for (auto vertex : domain.vertices)
        std::cout << "\tVERTEX " << vertex.first << " " << vertex.second.x << " " << vertex.second.y << std::endl;

    for (auto boundary : domain.boundaries)
    {
        std::string type_str = boundary.second.type == boundary_type::FREE ? "FREE" :
                               boundary.second.type == boundary_type::FIXED ? "FIXED" :
                               boundary.second.type == boundary_type::USER ? "USER" : "UNKNOWN";
        std::cout << "\tBOUNDARY " << boundary.first << " " << boundary.second.v1 << " " << boundary.second.v2 << " " << type_str << std::endl;
    }
    std::cout << "#########################" << std::endl;
} 

