#include <stdlib.h>

#include "pasta_config_parser.h"

static void *(*allocator)(size_t bytes) = malloc;

#ifdef TEST
void pasta_config_set_allocator(void *(*alloc_func)(size_t bytes)) { allocator = alloc_func; }
void pasta_config_reset_allocator() { allocator = malloc; }
#endif

Status pasta_config_load_modules(Module *mod[], int *loaded_modules, const char *filepath)
{
    return PASTA_ERROR_NOT_IMPLEMENTED;
}
