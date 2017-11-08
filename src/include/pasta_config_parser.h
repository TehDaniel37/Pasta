/*
 * pasta_config_parser.h
 *
 * Responsible for loading and parsing the configuration files containing 
 * information about the modules to be run.
 */
#ifndef PASTA_CONFIG_PARSER_H
#define PASTA_CONFIG_PARSER_H

#include "pasta_module.h"
#include "pasta_status_codes.h"

#ifdef TEST
void pasta_config_set_allocator(void *(*alloc_func)(size_t bytes));
void pasta_config_reset_allocator();
#endif

/*
 * pasta_load_modules
 * 
 * Opens, reads and parses the specified configuration file and returns a 
 * dynamically allocated array of the Modules that was loaded. 
 */
Status pasta_config_load_modules(Module *mod[], int *loaded_modules, const char *filepath);

#endif /* PASTA_CONFIG_PARSER_H */
