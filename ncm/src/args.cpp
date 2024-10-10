#include "args.hpp"

unrecognised_argument::unrecognised_argument(std::string const& arg)
    : std::runtime_error("Unrecognised argument: " + arg) {}

missing_argument_value::missing_argument_value(std::string const& arg)
    : std::runtime_error("Missing value after argument: " + arg) {}

program_arguments_t parse_command_line(int argc, char* argv[])
{
    program_arguments_t args = {
        .par_file = "mesh.par",
        .output_file = "dam.dat"
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
        else if (std::strcmp(arg, "--out") == 0)
        {
            if (++i < argc)
                args.output_file = argv[i];
            else
                throw missing_argument_value(arg);
        }
        else
            throw unrecognised_argument(arg);
    }

    return args;
}

