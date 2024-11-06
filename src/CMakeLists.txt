add_executable(engine
        core/defines.h
        core/log.h
        core/log.c
        main.c
        core/asserts.h
        platform/platform.h
        platform/platform_linux.c
        platform/platform_windows.c
)

find_package(Vulkan REQUIRED)

if (${PLATFORM_LINUX})
    find_package(X11 REQUIRED)
    set(PLATFORM_LIBRARIES X11::xcb)
endif()

target_include_directories(engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(engine PUBLIC
        Vulkan::Vulkan
        ${PLATFORM_LIBRARIES}
)
