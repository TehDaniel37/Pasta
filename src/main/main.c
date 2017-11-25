#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>

#include "schedr_job.h"
#include "schedr_scheduler.h"
#include "schedr_config_parser.h"
#include "schedr_status_codes.h"

#define CONFIG_FILE "/home/danalm/.config/schedr/schedr.conf"

int main(void)
{
    Job *jobs = NULL;
    int number_of_jobs = 0;
    Status status;
    
    // Append $HOME/.config/schedr/bin to PATH so user defined scripts can be executed
    // without using absolute paths
    schedr_scheduler_set_path();
    
    // Load jobs from config file
    status = schedr_config_load_jobs(&jobs, &number_of_jobs, CONFIG_FILE);
    
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
