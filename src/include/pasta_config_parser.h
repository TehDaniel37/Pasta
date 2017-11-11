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
 *
 * returns  PASTA_ERROR_INVALID_ARGUMENT if 'modules' is NOT NULL,
 *          PASTA_ERROR_CONFIG_FORMAT if the loaded config file has incorrect formatting,
 *          PASTA_ERROR_FILE_NOT_FOUND if the file at 'filepath' could not be found,
 *          PASTA_ERROR_PERMISSION_DENIED if the program did not have permission to open the file at 'filepath',
 *          PASTA_ERROR_ALLOCATION_FAILED if allocation of resources failed,
 *          PASTA_WARNING_NO_MODULES if there were no modules specified in the configuration file,
 *          PASTA_SUCCESS otherwise
 */
Status pasta_config_load_modules(Module *modules[], int *loaded_modules, const char *filepath);

#endif /* PASTA_CONFIG_PARSER_H */
