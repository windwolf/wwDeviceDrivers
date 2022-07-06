#include "../inc/common/safe_printf.h"
#include "os.h"

static DRIVER_MUTEX _printfMutex;

void safe_printf_init()
{
    driver_mutex_create(&_printfMutex, "_printfMutex");
}

void _begin_safe_printf()
{
    driver_mutex_get(&_printfMutex);
}

void _end_safe_printf()
{
    driver_mutex_put(&_printfMutex);
}