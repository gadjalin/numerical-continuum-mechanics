#ifndef NCM_ARGS_HPP
#define NCM_ARGS_HPP

#include <string>

#include "error.hpp"

typedef struct program_arguments
{
    std::string par_file;
} program_arguments_t;

class unknown_argument : public error
{
public:
    explicit unknown_argument(std::string const& arg);
};

class missing_argument_value : public error
{
public:
    explicit missing_argument_value(std::string const& arg);
};

program_arguments_t parse_command_line(int argc, char* argv[]);
void check_arguments(program_arguments_t const& args);

#endif // NCM_ARGS_HPP

