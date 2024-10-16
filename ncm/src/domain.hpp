#ifndef NCM_MESH_HPP
#define NCM_MESH_HPP

#include <string>
#include <map>
#include "error.hpp"

enum class boundary_type : uint8_t
{ 
    FREE = 0,
    FIXED,
    USER
};

typedef struct vertex
{
    float x, y;
} vertex_t;

typedef struct boundary
{
    std::string v1, v2;
    boundary_type type;
} boundary_t;

typedef struct domain
{
    std::map<std::string, vertex_t> vertices;
    std::map<std::string, boundary_t> boundaries;
} domain_t;

class multiple_definitions_error : public error
{
public:
    multiple_definitions_error(std::string const& filename, std::string const& name);
};

domain_t read_domain_file(std::string const& filename);

#endif // NCM_MESH_HPP
