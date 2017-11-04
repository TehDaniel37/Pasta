/*
 * pasta_config_parser.h
 *
 * Responsible for loading and parsing the configuration files containing 
 * information about the modules to be run.
 */
#ifndef PASTA_CONFIG_PARSER_H
#define PASTA_CONFIG_PARSER_H

#include "pasta_module.h"

/*
 * pasta_load_modules
 * 
 * Opens, reads and parses the specified configuration file and returns a 
 * dynamically allocated array of the Modules that was loaded. 
 */
Module* pasta_load_modules(const char *filepath); 

#endif /* PASTA_CONFIG_PARSER_H */
