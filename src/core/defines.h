#pragma once
#include <features.h>
#include <stdbool.h>

#if !defined __USE_ISOC11
        #error "Compile with C11"
#endif

#define static_assert _Static_assert

#if !defined(PLATFORM_LINUX)
#if defined(__linux__) || defined(__gnu_linux__)
        #define PLATFORM_LINUX 1
#endif
#endif

#if !defined(PLATFORM_MACOS)
#if defined(__APPLE__) || defined(TARGET_OS_MAC)
        #define PLATFORM_MACOS 1
#endif
#endif

#if !defined(PLATFORM_WINDOWS)
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
        #define PLATFORM_WINDOWS 1
        #ifndef _WIN64
                #error "Only 64-bit Windows is supported"
        #endif
#endif
#endif

#if !defined(PLATFORM_LINUX)
        #error "Currently, only Linux is supported."
#endif

#ifdef DYNLIB_EXPORT
// Export
        #ifdef _MSC_VER
                #define API __declspec(dllexport)
        #else
                #define API __attribute__((visibility("default")))
        #endif
#else
// Import
        #ifdef _MSC_VER
                #define API __declspec(dllimport)
        #else
                #define API
        #endif
#endif


typedef unsigned char           u8;
typedef unsigned short          u16;
typedef unsigned int            u32;
typedef unsigned long long      u64;

typedef signed char             i8;
typedef signed short            i16;
typedef signed int              i32;
typedef signed long long        i64;

typedef float                   f32;
typedef double                  f64;

typedef i32                     bool32;


static void defines_asserts() {
        static_assert(sizeof(u8) == 1, "sizeof(u8) != 1");
        static_assert(sizeof(u16) == 2, "sizeof(u16) != 2");
        static_assert(sizeof(u32) == 4, "sizeof(32) != 4");
        static_assert(sizeof(u64) == 8, "sizeof(u8) != 8");

        static_assert(sizeof(i8) == 1, "sizeof(i8) != 1");
        static_assert(sizeof(i16) == 2, "sizeof(i16) != 2");
        static_assert(sizeof(i32) == 4, "sizeof(32) != 4");
        static_assert(sizeof(i64) == 8, "sizeof(i8) != 8");

        static_assert(sizeof(f32) == 4, "sizeof(f32) != 4");
        static_assert(sizeof(f64) == 8, "sizeof(f64) != 8");
}
