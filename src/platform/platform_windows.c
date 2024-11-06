#include <stdio.h>

#include "platform.h"
#include "core/asserts.h"
#include "core/log.h"
#if defined(PLATFORM_WINDOWS)

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "windowsx.h"

typedef struct Platform_Windows {
        HINSTANCE h_instance;
        HWND h_window;
} Platform_Windows;

static f64 processor_clock_frequency;
static LARGE_INTEGER start_time;
static Platform_Events fallback_platform_events = {};
static Platform_Events* internal_platform_events = &fallback_platform_events;

LRESULT CALLBACK windows_window_procedure(HWND h_window, UINT message, WPARAM w_param, LPARAM l_param) {
        switch (message){
        case WM_ERASEBKGND: {
                // Notify OS that erasing the background is handled by the app. This prevents flickering.
                return 1;
        }
        case WM_CLOSE: {
                internal_platform_events->window_should_close = true;
                return 0;
        }
        case WM_DESTROY: {
                PostQuitMessage(0);
                return 0;
        } break;
        case WM_SIZE: {
                RECT rect;
                GetClientRect(h_window, &rect);

        } break;
        case WM_KEYUP:
        case WM_KEYDOWN:
        case WM_SYSKEYUP:
        case WM_SYSKEYDOWN: {

        } break;
        case WM_MOUSEMOVE: {
                // const i32 x_pos = GET_X_LPARAM(l_param);
                // const i32 y_pos = GET_Y_LPARAM(l_param);
                // ...
        } break;
        case WM_MOUSEWHEEL: {
                // i32 dir = GET_WHEEL_DELTA_WPARAM(w_param);
                // if (dir != 0) {
                //         dir = (dir < 0) ? -1 : 1; // clamp
                // }
        } break;
        // case WM_LBUTTONDBLCLK
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        // case WM_RBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        // case WM_MBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP: {

        } break;

        default:
        }

        return DefWindowProcA(h_window, message, w_param, l_param);
}

bool platform_init(const Platform_Init_Info* init_info, Platform* platform) {
        platform->internal_state = malloc(sizeof(Platform_Windows));
        Platform_Windows* state = (Platform_Windows*)platform->internal_state;

        state->h_instance = GetModuleHandleA(NULL); // get instance handle to current (this) application

        const HICON h_icon = LoadIconA(NULL, IDI_APPLICATION);
        const WNDCLASSA window_class = {
                .style = CS_DBLCLKS,
                .lpfnWndProc = windows_window_procedure,
                .cbClsExtra = 0,
                .cbWndExtra = 0,
                .hInstance = state->h_instance,
                .hIcon = h_icon,
                .hCursor = LoadCursorA(NULL, IDC_ARROW),
                .hbrBackground = NULL,
                .lpszMenuName = NULL,
                .lpszClassName = "orange_window_class",
        };
        if (!RegisterClassA(&window_class)) {
                MessageBoxA(NULL, "Failed to register window class!", "Error", MB_OK);
                return false;
        }

        const u32 window_style =
                WS_OVERLAPPED
                | WS_SYSMENU
                | WS_CAPTION
                | WS_MAXIMIZEBOX
                | WS_MINIMIZEBOX
                | WS_THICKFRAME;

        const u32 window_ex_style = WS_EX_APPWINDOW;

        RECT window_rect_with_border = {
                .left = init_info->window_x,
                .top = init_info->window_y,
                .right = init_info->window_x + init_info->window_width,
                .bottom = init_info->window_y + init_info->window_height,
        };
        AdjustWindowRectEx(&window_rect_with_border, window_style, false, window_ex_style);

        const i32 window_x = window_rect_with_border.left;
        const i32 window_y = window_rect_with_border.top;
        const i32 window_width = window_rect_with_border.right - window_rect_with_border.left;
        const i32 window_height = window_rect_with_border.bottom - window_rect_with_border.top;

        state->h_window = CreateWindowExA(
                        window_ex_style,
                        "orange_window_class",
                        init_info->window_title,
                        window_style,
                        window_x,
                        window_y,
                        window_width,
                        window_height,
                        NULL,
                        NULL,
                        state->h_instance,
                        NULL);

        if (state->h_window == NULL){
                MessageBoxA(NULL, "Failed to create window", "Error!", MB_ICONEXCLAMATION | MB_OK);
                UnregisterClassA("orange_window_class", state->h_instance);
                return false;
        }

        (void)ShowWindow(state->h_window, SW_SHOW);

        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        processor_clock_frequency = 1.0 / (f64)frequency.QuadPart; // convert 64-bit integer to float
        QueryPerformanceCounter(&start_time);

        return true;
}

void platform_deinit(Platform* platform) {
        if (platform == NULL) {
                return;
        }

        Platform_Windows* state = (Platform_Windows*)platform->internal_state;
        assertion(state != NULL);

        UnregisterClassA("orange_window_class", state->h_instance);
        if (state->h_window) DestroyWindow(state->h_window);

        free(state);

        *platform = (Platform){};
}

bool platform_poll_events(Platform* platform, Platform_Events* platform_events) {
        internal_platform_events = platform_events;

        MSG message;
        while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)){
                TranslateMessage(&message);
                DispatchMessageA(&message);
        }

        internal_platform_events = &fallback_platform_events;

        return !platform_events->window_should_close;
}

static void internal_platform_console_write(const HANDLE h_console, const char* message, const u8 log_level) {
        CONSOLE_SCREEN_BUFFER_INFO console_screen_buffer_info;
        GetConsoleScreenBufferInfo(h_console, &console_screen_buffer_info);

        const u8 colors[LOG_LEVEL_COUNT] = {64, 4, 6, 2, 1, 8};
        SetConsoleTextAttribute(h_console, colors[log_level]);

        OutputDebugString(message);
        const DWORD string_length = strlen(message);
        DWORD num_bytes_written = 0;
        WriteConsole(h_console, message, string_length, &num_bytes_written, NULL);

        SetConsoleTextAttribute(h_console, console_screen_buffer_info.wAttributes);
}

void platform_console_write(const char* message, const u8 log_level) {
        internal_platform_console_write(GetStdHandle(STD_OUTPUT_HANDLE), message, log_level);
}

void platform_console_write_error(const char* message, u8 log_level) {
        internal_platform_console_write(GetStdHandle(STD_ERROR_HANDLE), message, log_level);
}

f64 platform_get_absolute_time() {
        LARGE_INTEGER clock_cycles_passed;
        QueryPerformanceCounter(&clock_cycles_passed); // number of cycles since app start
        const f64 now = (f64)(clock_cycles_passed.QuadPart) * processor_clock_frequency;
        return now;
}

void platform_thread_sleep(const u64 milliseconds) {
        Sleep(milliseconds);
}

void* platform_memory_allocate(const u64 size) {
        return malloc(size);
}

void platform_memory_free(const void* ptr) {
        free((void*)ptr);
}

void platform_memory_zero(void* dest, const u64 size) {
        memset(dest, 0, size);
}

void platform_memory_set(void* dest, const int value, const u64 size) {
        memset(dest, value, size);
}

void platform_memory_copy(void* dest, const void* src, u64 size) {
        memcpy(dest, src, size);
}

#endif