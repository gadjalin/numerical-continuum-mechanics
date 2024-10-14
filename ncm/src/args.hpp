#ifndef NCM_ARGS_HPP
#define NCM_ARGS_HPP

#include <string>
#include <stdexcept>

typedef struct program_arguments
{
    std::string par_file;
} program_arguments_t;

class unrecognised_argument : public std::runtime_error
{
public:
    explicit unrecognised_argument(std::string const& arg);
};

class missing_argument_value : public std::runtime_error
{
public:
    explicit missing_argument_value(std::string const& arg);
};

program_arguments_t parse_command_line(int argc, char* argv[]);

#endif // NCM_ARGS_HPP
