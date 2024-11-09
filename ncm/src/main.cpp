#include "debug.hpp"
#include "args.hpp"
#include "config.hpp"
#include "domain.hpp"

int main(int argc, char* argv[])
{
    program_arguments_t args = parse_command_line(argc, argv);
    check_arguments(args);

    config_t cfg = read_config_file(args.par_file);
    validate_config(cfg);

#ifdef _DEBUG
    print_config(cfg);
#endif

    domain_t domain = read_domain_file(cfg.domain_file);

#ifdef _DEBUG
    print_domain(domain);
#endif

    return 0;
}

