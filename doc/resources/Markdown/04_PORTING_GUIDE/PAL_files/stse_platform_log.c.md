# stse_platform_log.c {#stse_platform_log}

The `stse_platform_log.c` file provides string formatting and logging functions for the STSE library, abstracting the platform-specific details of standard output and debugging streams in order to don't be `stdio`-dependent and allow platform-specific implementations for logging and output.

## stse_platform_printf:
- **Purpose**: Provides a platform abstraction for formatted output, replacing all direct calls to `printf()` in the library with this function to allow platform-specific implementations and avoid direct use of standard I/O in library code.
- **Parameters**: Accepts a format string and a variable number of arguments, similar to the standard `printf()` function.
- **Return Value**: return void

## Implementation example:

```c
#include "stselib.h"

#include <stdio.h>
#include <stdarg.h>

void stse_platform_printf(const char *format, ...) {
    va_list args;
    
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}
```
