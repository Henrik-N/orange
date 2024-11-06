#pragma once
#include "defines.h"

void asserts_log_output(const char* file_name, i32 line, const char* expression, const char* message);

#define RUNTIME_ASSERTS_ENABLE 1

#if RUNTIME_ASSERTS_ENABLE

#if _MSC_VER__
        #include <intrin.h>
        #define asserts_debug_break() __debugBreak()
#else
        #define asserts_debug_break() __builtin_trap()
#endif

#define assertion(expression) \
        if (expression); else { \
                asserts_log_output(__FILE__, __LINE__, #expression, ""); \
                asserts_debug_break(); \
        }

#define assertion_msg(expression, message) \
        if (expression); else { \
                asserts_log_output(__FILE__, __LINE__, #expression, message); \
                asserts_debug_break(); \
        }

#else // #if RUNTIME_ASSERTS_ENABLE == 0

#define assertion(expression)
#define assertion_msg(expression, message, ...)

#endif
