include(FetchContent)

FetchContent_Declare(sdl2
        GIT_REPOSITORY  https://github.com/libsdl-org/SDL
        GIT_TAG         release-2.0.16
        GIT_SHALLOW     TRUE
        SOURCE_DIR      ${CMAKE_BINARY_DIR}/sdl2-2.0.16-src
        )

FetchContent_GetProperties(sdl2)
if(NOT sdl2_POPULATED)
    message(STATUS "Fetching sdl2 2.0.16")
    FetchContent_Populate(sdl2)
    message(STATUS "Fetched sdl2 2.0.16")
endif()

set(BUILD_SHARED_LIBS ON)
set(VIDEO_KMSDRM ON)
add_subdirectory(${sdl2_SOURCE_DIR})
