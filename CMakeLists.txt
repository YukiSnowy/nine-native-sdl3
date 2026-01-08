cmake_minimum_required(VERSION 3.5)

set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)

project(nine-native)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)

set(SDL3DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/SDL3-3.2.26)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_X11 ON CACHE BOOL "" FORCE)
set(SDL_WAYLAND OFF CACHE BOOL "" FORCE)
add_subdirectory(${SDL3DIR})

if (NINE_POSITION_INDEPENDENT)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

add_library(${PROJECT_NAME} STATIC
    include/nine_sdl.h
    src/nine_sdl.c
    src/dri3.c
    src/dri3.h
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${SDL3DIR}/include
    include
    include/D3D9
    include/windows
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3-static
)

add_executable(triangle_SDL testbed/triangle_SDL.cpp)
target_include_directories(triangle_SDL PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/include/D3D9
${CMAKE_CURRENT_SOURCE_DIR}/include/windows)
target_link_libraries(triangle_SDL PRIVATE -L"${CMAKE_CURRENT_SOURCE_DIR}/lib" -lSDL3 -lnine-native)
set_target_properties(triangle_SDL
PROPERTIES
BUILD_RPATH "$ORIGIN"
INSTALL_RPATH "$ORIGIN"
RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
