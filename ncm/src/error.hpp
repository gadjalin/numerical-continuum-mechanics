#ifndef NCM_ERROR_HPP
#define NCM_ERROR_HPP

#include <stdexcept>

class error : public std::logic_error
{
public:
    error(std::string const& what);
};

#endif // NCM_ERROR_HPP

