#ifndef NCM_CONFIG_HPP
#define NCM_CONFIG_HPP

#include <string>

typedef struct config
{
    std::string output_file;
    std::string domain_file;
    float young_modulus;
    float poisson_ratio;
    float density;
    float gravity;
    float resolution;
} config_t;

config_t read_config_file(std::string const& filename);
void validate_config(config_t const& cfg);

#endif // NCM_CONFIG_HPP

