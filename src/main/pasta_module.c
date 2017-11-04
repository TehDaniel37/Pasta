#include <stdlib.h>

#include "pasta_module.h"

#ifdef TEST
void pasta_module_set_allocator(void *(*alloc_func)(size_t bytes)) { }
#endif

Status pasta_module_create(Module *module_p)
{
    return PASTA_ERROR_NOT_IMPLEMENTED;
}

Status pasta_module_destroy(Module *const module_p)
{
    return PASTA_ERROR_NOT_IMPLEMENTED;
}

Status pasta_module_set_name(Module *const module_p, const char *name)
{
    return PASTA_ERROR_NOT_IMPLEMENTED;
}

Status pasta_module_set_command(Module *const module_p, const char *command)
{
    return PASTA_ERROR_NOT_IMPLEMENTED;
}

Status pasta_module_set_interval(Module *const module_p, int interval)
{
    return PASTA_ERROR_NOT_IMPLEMENTED;
}

Status pasta_module_set_state(Module *const module_p, ModuleState state)
{
    return PASTA_ERROR_NOT_IMPLEMENTED;
}
