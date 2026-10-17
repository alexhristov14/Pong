cmake_minimum_required(VERSION 3.24)

project(Pong
    VERSION 0.1.0
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# --- SDL3 -------------------------------------------------------------------
# Use an installed SDL3 if one exists, otherwise download and build it.
include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        release-3.4.18
    GIT_SHALLOW    TRUE
    FIND_PACKAGE_ARGS NAMES SDL3 CONFIG
)

set(SDL_SHARED ON  CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SDL3)

# --- Game executable --------------------------------------------------------
file(GLOB_RECURSE PONG_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(pong ${PONG_SOURCES})

target_include_directories(pong PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(pong PRIVATE SDL3::SDL3)

target_compile_options(pong PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Put the SDL3 shared library next to the executable (needed on Windows).
if(WIN32)
    add_custom_command(TARGET pong POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3> $<TARGET_FILE_DIR:pong>
    )
endif()
