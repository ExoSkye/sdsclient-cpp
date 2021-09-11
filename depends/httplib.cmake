include(FetchContent)

FetchContent_Declare(httplib
        GIT_REPOSITORY  https://github.com/yhirose/cpp-httplib
        GIT_SHALLOW     TRUE
        SOURCE_DIR      ${CMAKE_BINARY_DIR}/cpp-httplib-0.9.4-src
        )

FetchContent_GetProperties(httplib)

if(NOT httplib_POPULATED)
    message(STATUS "Fetching httplib 0.9.4")
    FetchContent_Populate(httplib)
    message(STATUS "Fetched httplib 0.9.4")
endif()
add_subdirectory(${httplib_SOURCE_DIR})