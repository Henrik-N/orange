add_executable(engine
        core/defines.h
        core/log.h
        core/log.c
        main.c
        core/asserts.h
        platform/platform.h
        platform/platform_linux.c
)

find_package(Vulkan REQUIRED)
find_package(X11 REQUIRED)

target_include_directories(engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(engine PUBLIC
        Vulkan::Vulkan
        X11::xcb
)
