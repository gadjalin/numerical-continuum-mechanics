#ifndef NCM_DEBUG_HPP
#define NCM_DEBUG_HPP

typedef struct config config_t;
typedef struct domain domain_t;

void print_config(config_t const& cfg);
void print_domain(domain_t const& domain);

#endif // NCM_DEBUG_HPP
