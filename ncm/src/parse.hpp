#ifndef NCM_PARSE_HPP
#define NCM_PARSE_HPP

#include <string>
#include <stdexcept>

#define PARFILE_COMMENT_CHAR '#'

class invalid_file_format : public std::runtime_error
{
public:
    invalid_file_format(std::string const& parfile, std::size_t line_nr, std::string const& expr);
};

class unknown_file_key : public std::runtime_error
{
public:
    unknown_file_key(std::string const& file, std::size_t line_nr, std::string const& key);
};

[[nodiscard]]
std::string trim_comment(std::string const& line);

[[nodiscard]]
std::string trim(std::string const& line);

#endif // NCM_PARSE_HPP

