#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>

#include "schedr_job.h"
#include "schedr_scheduler.h"
#include "schedr_config_parser.h"
#include "schedr_status_codes.h"

// TODO: This should not be here
Status get_config_path(char **config_out)
{
    const char home_rel[] = "/.config/schedr/schedr.conf";
    const char config_home_rel[] = "/schedr/schedr.conf";
    char *rel = (char *)config_home_rel;

    char *config_dir = getenv("XDG_CONFIG_HOME");

    if (config_dir == NULL) {
        config_dir = getenv("HOME");
        rel = (char *)home_rel;

        if (config_dir == NULL) { return SCHEDR_FAILURE; }
    }

    int len = sizeof (rel) + strlen(config_dir);
    *config_out = (char *)malloc(sizeof (char) * len);
    
    strcpy(*config_out, config_dir);
    strcat(*config_out, rel);
    *(config_out[len]) = '\0';

    return SCHEDR_SUCCESS;
}

int main(int argc, char *argv[])
{
    char *config_path;
    Job *jobs = NULL;
    int number_of_jobs = 0;

    Status status = get_config_path(&config_path);

    if (status != SCHEDR_FAILURE) {
        puts("Failed to get config path. Neither HOME or XDG_CONFIG_HOME is set");
        exit(SCHEDR_FAILURE);
    }
    
    // Append $HOME/.config/schedr/bin to PATH so user defined scripts can be executed
    // without using absolute paths
    schedr_scheduler_set_path();
    
    // Load jobs from config file
    status = schedr_config_load_jobs(&jobs, &number_of_jobs, config_path);
    free(config_path);

    if (status != SCHEDR_SUCCESS)
    {
        printf("Could not load config files. Error code: %d\n", status);
        exit(EXIT_FAILURE);
    }
    
    // Start the jobs
    for (int i = 0; i < number_of_jobs; i++)
    {
        if ((status = schedr_scheduler_start_job(&(jobs[i]))) != SCHEDR_SUCCESS) 
        {
            printf("Could not start job nr %d. Error code: %d\n", i + 1, status);
            exit(EXIT_FAILURE);
        }
    }
    
    pause();    // Wait for termination signal
    
    // Stop the jobs before terminating
    for (int i = 0; i < number_of_jobs; i++)
    {
        if ((status = schedr_scheduler_stop_job(&(jobs[i]))) != SCHEDR_SUCCESS) 
        {
            printf("Could not stop job nr %d. Error code: %d\n", i + 1, status);
            exit(EXIT_FAILURE);
        }
    }
    
    return SCHEDR_SUCCESS;
}
