include(FetchContent)

FetchContent_Declare(jsoncpp
        GIT_REPOSITORY  https://github.com/open-source-parsers/jsoncpp
        GIT_TAG         1.9.4
        GIT_SHALLOW     TRUE
        SOURCE_DIR      ${CMAKE_BINARY_DIR}/jsoncpp-1.9.4-src
        )

FetchContent_GetProperties(jsoncpp)

if(NOT jsoncpp_POPULATED)
    message(STATUS "Fetching jsoncpp 1.9.4")
    FetchContent_Populate(jsoncpp)
    message(STATUS "Fetched jsoncpp 1.9.4")
endif()
add_subdirectory(${jsoncpp_SOURCE_DIR})