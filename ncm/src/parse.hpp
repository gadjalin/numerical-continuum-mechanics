#ifndef NCM_PARSE_HPP
#define NCM_PARSE_HPP

#include <string>
#include "error.hpp"

#define PARFILE_COMMENT_CHAR '#'

typedef struct file_location
{
    std::string file;
    size_t line;
    std::string expr;
} file_location_t;

class invalid_file_format : public error
{
public:
    invalid_file_format(file_location_t const& loc);
};

class invalid_file_expression : public error
{
public:
    invalid_file_expression(file_location_t const& loc);
};

[[nodiscard]]
std::string trim_comment(std::string const& line);

[[nodiscard]]
std::string trim(std::string const& line);

#endif // NCM_PARSE_HPP

