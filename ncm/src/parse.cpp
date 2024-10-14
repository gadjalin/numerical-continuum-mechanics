#include "parse.hpp"

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

invalid_file_format::invalid_file_format(std::string const& parfile, std::size_t line_nr, std::string const& line)
    : std::runtime_error("Invalid format in file " + parfile + " @ line " + std::to_string(line_nr) + ": " + line) {}

unknown_file_key::unknown_file_key(std::string const& file, std::size_t line_nr, std::string const& key)
    : std::runtime_error("Unknown key in file " + file + " @ line " + std::to_string(line_nr) + ": " + key) {}

