#pragma once
#include "defines.h"

#if IS_RELEASE_BUILD
        #define LOG_LEVEL_WARNING_ENABLE 0
        #define LOG_LEVEL_DEBUG_ENABLE 0
        #define LOG_LEVEL_TRACE_ENABLE 0
#else
        #define LOG_LEVEL_DEBUG_ENABLE 1
        #define LOG_LEVEL_WARNING_ENABLE 1
        #define LOG_LEVEL_TRACE_ENABLE 1
#endif

typedef enum Log_Level {
        LOG_LEVEL_FATAL = 0, 
        LOG_LEVEL_ERROR,
        LOG_LEVEL_WARNING,
        LOG_LEVEL_INFO,
        LOG_LEVEL_DEBUG,
        LOG_LEVEL_TRACE,
        LOG_LEVEL_COUNT,
} Log_Level;

bool log_init();
void log_deinit();

void log_output(Log_Level log_level, const char* format_string, ...);

#if LOG_LEVEL_TRACE_ENABLE
        #define log_trace(format_string, ...)           log_output(LOG_LEVEL_TRACE, format_string __VA_OPT__(,) __VA_ARGS__)
#else
        #define log_trace(format_string, ...)
#endif

#if LOG_LEVEL_DEBUG_ENABLE
        #define log_debug(format_string, ...)           log_output(LOG_LEVEL_DEBUG, format_string __VA_OPT__(,) __VA_ARGS__)
#else
        #define log_debug(format_string, ...)
#endif

#define log_info(format_string, ...)                    log_output(LOG_LEVEL_INFO, format_string __VA_OPT__(,) __VA_ARGS__)

#if LOG_LEVEL_WARNING_ENABLE
        #define log_warning(format_string, ...)         log_output(LOG_LEVEL_WARNING, format_string __VA_OPT__(,) __VA_ARGS__)
#else
        #define log_warning(format_string, ...)
#endif

#define log_error(format_string, ...)                   log_output(LOG_LEVEL_ERROR, format_string __VA_OPT__(,) __VA_ARGS__)
#define log_fatal(format_string, ...)                   log_output(LOG_LEVEL_FATAL, format_string __VA_OPT__(,) __VA_ARGS__)

