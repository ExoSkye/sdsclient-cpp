cmake_minimum_required(VERSION 3.14)
project(SDSClient)

set(CMAKE_CXX_STANDARD 20)

include(depends/httplib.cmake)
include(depends/jsoncpp.cmake)
#include(depends/sdl2.cmake) - NOT NEEDED - CALLED BY SDL_IMAGE
include(depends/sdl2-image.cmake)
include(depends/b64.cmake)

add_executable(SDSClient main.cpp)
target_link_libraries(SDSClient jsoncpp SDL2 httplib sdl2-image-lib cpp-base64)