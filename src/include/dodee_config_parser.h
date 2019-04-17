/*
 * dodee_config_parser.h
 *
 * Responsible for loading and parsing the configuration files containing 
 * information about the tasks to be run.
 */
#ifndef DODEE_CONFIG_PARSER_H
#define DODEE_CONFIG_PARSER_H

#include "dodee_task.h"
#include "dodee_status_codes.h"

#ifdef TEST
void dodee_config_set_allocator(void *(*alloc_func)(size_t bytes));
void dodee_config_reset_allocator();
void dodee_config_set_on_number_of_tasks_found_hook(void (*hook)(int expected_dodee));
void dodee_config_remove_on_number_of_tasks_found_hook();
#endif

/*
 * dodee_config_load_tasks
 * 
 * Opens, reads and parses the specified configuration file and returns a 
 * dynamically allocated array of the tasks that was loaded.
 *
 * returns  DODEE_ERROR_INVALID_ARGUMENT if 'tasks' is NOT NULL,
 *          DODEE_ERROR_CONFIG_FORMAT if the loaded config file has incorrect formatting,
 *          DODEE_ERROR_FILE_NOT_FOUND if the file at 'filepath' could not be found,
 *          DODEE_ERROR_PERMISSION_DENIED if the program did not have permission to open the file at 'filepath',
 *          DODEE_ERROR_ALLOCATION_FAILED if allocation of resources failed,
 *          DODEE_WARNING_NO_TASKS if there were no tasks specified in the configuration file,
 *          DODEE_SUCCESS otherwise
 */
DodeeStatus dodee_config_load_tasks(DodeeTask *tasks[], int *loaded_tasks, const char *filepath);

#endif /* DODEE_CONFIG_PARSER_H */
