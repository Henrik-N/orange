#include "log.h"
#include <stdarg.h>
#include <stdio.h>
#include "../platform/platform.h"

bool log_init() {
        // TODO: Create .log output file etc
        return true;
}

void log_deinit() {
        //
}

void log_output(const Log_Level log_level, const char* format_string, ...) { // NOLINT(*-no-recursion)
        if (format_string == NULL) {
                log_output(LOG_LEVEL_FATAL, "[[Empty format string - fix me]]");
                return;
        }

        const char* log_level_strings[LOG_LEVEL_COUNT] = {
                "FATAL",
                "ERROR",
                "WARN",
                "INFO",
                "DEBUG",
                "TRACE",
        };

        char formatted_string[1024 * 4];
        {
                va_list args;
                va_start(args, format_string);
                vsnprintf(formatted_string, sizeof(formatted_string), format_string, args);
                va_end(args);
        }

        char formatted_string_prefixed[1024 * 5];
        {
                sprintf(formatted_string_prefixed, "[%s]\t%s\n", log_level_strings[log_level], formatted_string);
        }

        const bool is_error = log_level < LOG_LEVEL_WARNING;
        if (is_error) {
                platform_console_write(formatted_string_prefixed, log_level);
        } else {
                platform_console_write_error(formatted_string_prefixed, log_level);
        }
}

void asserts_log_output(const char* file_name, const i32 line, const char* expression, const char* message) {
        log_output(LOG_LEVEL_FATAL, "[%s:%d] Assertion (%s) failed. %s", file_name, line, expression, message);
}

