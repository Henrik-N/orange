# cmake_minimum_required(VERSION 3.30)
cmake_minimum_required(VERSION 3.22.1)

project(orange C)
set(CMAKE_C_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # enable compile_commands.json export - clangd lsp can use this
set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DIS_DEBUG_BUILD")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DIS_RELEASE_BUILD")

set(PLATFORM_LINUX   (${CMAKE_SYSTEM_NAME} STREQUAL "Linux"))
set(PLATFORM_WINDOWS (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))

if (PLATFORM_LINUX)
    add_compile_definitions(PLATFORM_LINUX=1)
endif()

add_subdirectory(src)
