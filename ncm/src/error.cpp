#include "error.hpp"

error::error(std::string const& what)
    : std::logic_error(what) {}

