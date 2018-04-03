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

static char *get_config_path()
{
    //const char config_file_rel[] = "/.config/schedr/schedr.conf";
    const char config_file_rel[] = "/git/schedr/test.conf";
    char *home = getenv("HOME");
    int len = sizeof (config_file_rel) + strlen(home);
    char *config_path = (char *)malloc(sizeof (char) * len);
    
    strcpy(config_path, home);
    strncat(config_path, config_file_rel, sizeof (config_file_rel) - 1);
    config_path[len] = '\0';
    
    return config_path;
}

int main(int argc, char *argv[])
{
    char *config_path = get_config_path();
    Job *jobs = NULL;
    int number_of_jobs = 0;
    Status status;
    
    // Append $HOME/.config/schedr/bin to PATH so user defined scripts can be executed
    // without using absolute paths
    schedr_scheduler_set_path();
    
    // Load jobs from config file
    status = schedr_config_load_jobs(&jobs, &number_of_jobs, config_path);
    free(config_path);

    for (int i = 0; i < number_of_jobs; i++) {
        printf("Job %d:\nName: %s\nCommand: %s\nInterval: %d\n\n", i, jobs[i].name, jobs[i].command, jobs[i].interval_seconds);
    }

    exit(1);
    
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
