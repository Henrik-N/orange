#pragma once
#include "../core/defines.h"

typedef struct Platform {
        void* internal_state;
} Platform;

typedef struct Platform_Init_Info {
        const char* window_title;
        i32 window_x;
        i32 window_y;
        i32 window_width;
        i32 window_height; 
} Platform_Init_Info;

typedef struct Platform_Events {
        bool window_should_close;
} Platform_Events;

bool    platform_init(const Platform_Init_Info* init_info, Platform* platform);
void    platform_deinit(Platform* platform);
bool    platform_poll_events(Platform* platform, Platform_Events* platform_events);

void    platform_console_write(const char* message, u8 log_level);
void    platform_console_write_error(const char* message, u8 log_level);

f64     platform_get_absolute_time();

void    platform_thread_sleep(u64 milliseconds);

void*   platform_memory_allocate(u64 size);
void    platform_memory_free(const void* ptr);
void    platform_memory_zero(void* dest, u64 size);
void    platform_memory_set(void* dest, int value, u64 size);
void    platform_memory_copy(void* dest, const void* src, u64 size);
