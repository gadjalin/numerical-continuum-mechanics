#include <stdexcept>

#include "debug.hpp"
#include "args.hpp"
#include "config.hpp"
#include "domain.hpp"

int main(int argc, char* argv[])
{
    program_arguments_t args = parse_command_line(argc, argv);

    config_t cfg = read_par_file(args.par_file);
    if (cfg.output_file.empty()) throw std::runtime_error("Empty output file name!");
    if (cfg.domain_file.empty()) throw std::runtime_error("Empty mesh file name!");

#ifdef _DEBUG
    print_config(cfg);
#endif

    domain_t domain = read_domain_file(cfg.domain_file);

#ifdef _DEBUG
    print_domain(domain);
#endif

    return 0;
}

