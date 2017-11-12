/*
 * pasta_scheduler.h
 *
 * Responsible for starting, managing and stopping modules.
 */
#ifndef PASTA_SCHEDULER_H
#define PASTA_SCHEDULER_H

#include <stdbool.h>

#include <pasta_module.h>
#include <pasta_status_codes.h>

/*
 * pasta_scheduler_init
 *
 * Sets up the scheduler as it's own process. The calling process will be 
 * notified through the signal SIGUSR2 when the scheduler is ready to begin
 * starting modules.
 */
Status pasta_scheduler_init(void);

/*
 * pasta_scheduler_start_module
 *
 * Starts a new process that manages the provided module, executing it with the
 * interval provided in the module.
 *
 * The scheduler MUST be initialized through the pasta_scheduler_init before
 * attempting to start a module.
 */
Status pasta_scheduler_start_module(Module *module_p);

/*
 * pasta_scheduler_stop_module
 *
 * Stops the provided module if it is running.
 */
Status pasta_scheduler_stop_module(Module *module_p);

#endif /* PASTA_SCHEDULER_H */
