include(FetchContent)

FetchContent_Declare(cpp-base64
        GIT_REPOSITORY  https://github.com/ReneNyffenegger/cpp-base64
        GIT_TAG         V2.rc.08
        GIT_SHALLOW     TRUE
        SOURCE_DIR      ${CMAKE_BINARY_DIR}/b64-src
        )

FetchContent_GetProperties(cpp-base64)
if(NOT cpp-base64_POPULATED)
    message(STATUS "Fetching cpp-base64")
    FetchContent_Populate(cpp-base64)
    message(STATUS "Fetched cpp-base64")
endif()

add_library(cpp-base64-impl ${cpp-base64_SOURCE_DIR}/base64.cpp)

add_library(cpp-base64 INTERFACE)
target_include_directories(cpp-base64 INTERFACE ${cpp-base64_SOURCE_DIR})
target_link_libraries(cpp-base64 INTERFACE cpp-base64-impl)