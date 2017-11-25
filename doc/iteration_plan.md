# Schedr iteration plan

## Iteration 0
After this iteration the program should be able to:

- Load polled type jobs from the configuration file
- Implement functions for starting a job and stopping a job 
- The output from the jobs should be visible on stdout

The program should run the loaded jobs in parallell. Each job should be executed, and the program shall then wait `interval` number of seconds and then execute the job again, until the function to stop the job is called or the program exits. 
