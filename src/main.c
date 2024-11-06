#include "core/defines.h"
#include "core/log.h"
#include "platform/platform.h"

i32 main(void) {
        if (!log_init()) {
                return -1;
        }
        log_trace("Trace %d", 1337);
        log_info("Info %d", 1337);
        log_debug("Debug %d", 1337);
        log_warning("Warning %d", 1337);
        log_error("Error %d", 1337);
        log_fatal("Fatal %d", 1337);

        Platform platform;
        {
                const Platform_Init_Info platform_init_info = {
                        .window_title = "Orange",
                        .window_x = 100,
                        .window_y = 100,
                        .window_width = 800,
                        .window_height = 600,
                };
                if (!platform_init(&platform_init_info, &platform)) {
                        log_deinit();
                        return -2;
                }
        }

        Platform_Events events;
        while (platform_poll_events(&platform, &events)) {
                platform_thread_sleep(100);
        }


        platform_deinit(&platform);
        log_deinit();
        return 0;
}
