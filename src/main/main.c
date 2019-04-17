#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>

#include "dodee_task.h"
#include "dodee_scheduler.h"
#include "dodee_config_parser.h"
#include "dodee_status_codes.h"

// TODO: This should not be here
DodeeStatus get_config_path(char **config_out)
{
    const char home_rel[] = "/.config/dodee/dodee.conf";
    const char config_home_rel[] = "/dodee/dodee.conf";
    char *rel = (char *)config_home_rel;

    char *config_dir = getenv("XDG_CONFIG_HOME");

    if (config_dir == NULL) {
        config_dir = getenv("HOME");
        rel = (char *)home_rel;

        if (config_dir == NULL) { return DODEE_FAILURE; }
    }

    int len = sizeof (rel) + strlen(config_dir);
    *config_out = (char *)malloc(sizeof (char) * len);
    
    strcpy(*config_out, config_dir);
    strcat(*config_out, rel);
    *(config_out[len]) = '\0';

    return DODEE_SUCCESS;
}

int main(int argc, char *argv[])
{
    char *config_path;
    DodeeTask *tasks = NULL;
    int number_of_tasks = 0;

    DodeeStatus status = get_config_path(&config_path);

    if (status != DODEE_FAILURE) {
        puts("Failed to get config path. Neither HOME or XDG_CONFIG_HOME is set");
        exit(DODEE_FAILURE);
    }
    
    // Append $HOME/.config/dodee/bin to PATH so user defined scripts can be executed
    // without using absolute paths
    dodee_scheduler_set_path();
    
    // Load tasks from config file
    status = dodee_config_load_jobs(&tasks, &number_of_tasks, config_path);
    free(config_path);

    if (status != DODEE_SUCCESS)
    {
        printf("Could not load config files. Error code: %d\n", status);
        exit(EXIT_FAILURE);
    }
    
    // Start the tasks
    for (int i = 0; i < number_of_tasks; i++)
    {
        if ((status = dodee_scheduler_start_task(&(tasks[i]))) != DODEE_SUCCESS) 
        {
            printf("Could not start task nr %d. Error code: %d\n", i + 1, status);
            exit(EXIT_FAILURE);
        }
    }
    
    pause();    // Wait for termination signal
    
    // Stop the tasks before terminating
    for (int i = 0; i < number_of_tasks; i++)
    {
        if ((status = dodee_scheduler_stop_task(&(tasks[i]))) != DODEE_SUCCESS) 
        {
            printf("Could not stop task nr %d. Error code: %d\n", i + 1, status);
            exit(EXIT_FAILURE);
        }
    }
    
    return DODEE_SUCCESS;
}
