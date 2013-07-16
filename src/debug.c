#include "debug.h"

#include <stdarg.h>
#include <stdio.h>

#define UNUSED(x) (x = x)

void i2dash_debug_msg(const char *fmt, ...)
{
    #ifdef DEBUG
    char message[256];
    va_list args;
    va_start(args, fmt);
    vsprintf(message, fmt, args);
    va_end(args);
    printf("[i2dash][debug][msg]: %s\n", message);
    #else
    UNUSED(fmt);
    #endif
}

void i2dash_debug_err(const char *fmt, ...)
{
    char message[256];
    va_list args;
    va_start(args, fmt);
    vsprintf(message, fmt, args);
    va_end(args);
    printf("[i2dash][debug][err]: %s\n", message);
}
