#include "parse.hpp"

invalid_file_format::invalid_file_format(file_location_t const& loc)
    : error("Invalid format in file " + loc.file + " @ line " + std::to_string(loc.line) + ": " + loc.expr) {}

invalid_file_expression::invalid_file_expression(file_location_t const& loc)
    : error("Invalid expression in file " + loc.file + " @ line " + std::to_string(loc.line) + ": " + loc.expr) {}

[[nodiscard]]
std::string trim_comment(std::string const& line)
{
    std::size_t pos = line.find(PARFILE_COMMENT_CHAR);
    return line.substr(0, pos);
}

[[nodiscard]]
std::string trim(std::string const& line)
{
    std::size_t start = line.find_first_not_of(" \t");
    std::size_t end = line.find_last_not_of(" \t");
    if (start == std::string::npos)
        return "";
    else if (end != std::string::npos)
        return line.substr(start, end - start + 1);
    else
        return line.substr(start);
}

