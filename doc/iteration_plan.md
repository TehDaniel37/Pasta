# Schedr iteration plan

## Iteration 1
Iteration 1 will implement the following features:

- Reimplement parsing of config file to make it trivial to add new job types and other settings

- Calendar type scheduling of jobs.

Month format: `every month on <DAY OF MONTH> [at <TIME>]`

Week format: `every week on <WEEKDAY> [at <TIME>]`

Day Format: `every day [at <TIME>]`

For example:

```
        `every month on 13th`
        `every day at 8 pm`
        `every week on thursday`
```

If `at <TIME>` is left out, 8:00 is assumed.

If the computer is booted after a scheduled time has passed but not yet been executed, the program should execute the command as soon as possible. However, multiple instances of the same job will not be queued. If for example multiple days has passed since the last scheduled daily job, that job will only run once.

- Save last execution of `every` type jobs so the time until next execution is retained between reboots. For example if a job is scheduled to run every 6 hours and the computer shuts down after 3 hours. The next execution of that job will run after 3 hours after boot.

## Iteration 0 (Finished on 2017-11-26)
After this iteration the program should be able to:

- Load polled type jobs from the configuration file
- Implement functions for starting a job and stopping a job 
- The output from the jobs should be visible on stdout

The program should run the loaded jobs in parallell. Each job should be executed, and the program shall then wait `interval` number of seconds and then execute the job again, until the function to stop the job is called or the program exits. 
