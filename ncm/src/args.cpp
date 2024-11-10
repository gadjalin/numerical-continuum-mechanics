#include "args.hpp"

#include <cstring>

unknown_argument::unknown_argument(std::string const& arg)
    : error("Unrecognised argument: " + arg) {}

missing_argument_value::missing_argument_value(std::string const& arg)
    : error("Missing value after argument: " + arg) {}

program_arguments_t parse_command_line(int argc, char* argv[])
{
    program_arguments_t args = {
        .par_file = "mesh.par",
    };

    for (int i = 1; i < argc; ++i)
    {
        char const* arg = argv[i];

        if (std::strcmp(arg, "--par-file") == 0)
        {
            if (++i < argc)
                args.par_file = argv[i];
            else
                throw missing_argument_value(arg);
        }
        else
            throw unknown_argument(arg);
    }

    return args;
}

void check_arguments(program_arguments_t const& args)
{

}

