/*
 * schedr_config_parser.h
 *
 * Responsible for loading and parsing the configuration files containing 
 * information about the jobs to be run.
 */
#ifndef SCHEDR_CONFIG_PARSER_H
#define SCHEDR_CONFIG_PARSER_H

#include "schedr_job.h"
#include "schedr_status_codes.h"

#ifdef TEST
void schedr_config_set_allocator(void *(*alloc_func)(size_t bytes));
void schedr_config_reset_allocator();
#endif

/*
 * schedr_config_load_jobs
 * 
 * Opens, reads and parses the specified configuration file and returns a 
 * dynamically allocated array of the Jobs that was loaded.
 *
 * returns  SCHEDR_ERROR_INVALID_ARGUMENT if 'jobs' is NOT NULL,
 *          SCHEDR_ERROR_CONFIG_FORMAT if the loaded config file has incorrect formatting,
 *          SCHEDR_ERROR_FILE_NOT_FOUND if the file at 'filepath' could not be found,
 *          SCHEDR_ERROR_PERMISSION_DENIED if the program did not have permission to open the file at 'filepath',
 *          SCHEDR_ERROR_ALLOCATION_FAILED if allocation of resources failed,
 *          SCHEDR_WARNING_NO_JOBS if there were no jobs specified in the configuration file,
 *          SCHEDR_SUCCESS otherwise
 */
Status schedr_config_load_jobs(Job *jobs[], int *loaded_jobs, const char *filepath);

#endif /* SCHEDR_CONFIG_PARSER_H */
