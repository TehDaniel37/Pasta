#include <stdio.h>
#include <stdlib.h>

#include "pasta_error.h"
#include "pasta_module.h"

int main(void)
{
    Module *module = (Module *)malloc(sizeof (Module));

    const char name[] = "test module";
    const char cmd[] = "echo 'testing 1, 2, 3'";
    int interval = 1;
    ModuleState state = Stopped;

    Status status = pasta_module_set_name(module, name, sizeof (name));
    if (status != PASTA_SUCCESS) { return status; }

    printf("name: %s\n", module->name);

    status = pasta_module_set_command(module, cmd, sizeof (cmd));
    if (status != PASTA_SUCCESS) { return status; }

    printf("command: %s\n", module->command);

    status = pasta_module_set_interval(module, interval);
    if (status != PASTA_SUCCESS) { return status; }

    printf("interval: %d\n", module->interval_seconds);

    status = pasta_module_set_state(module, state);
    if (status != PASTA_SUCCESS) { return status; }

    printf("state: %d\n", module->state);

    return PASTA_SUCCESS;
}
