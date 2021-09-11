include(FetchContent)
include(depends/sdl2.cmake)

FetchContent_Declare(sdl2-image
        GIT_REPOSITORY  https://github.com/libsdl-org/SDL_image
        GIT_TAG         release-2.0.5
        GIT_SHALLOW     TRUE
        SOURCE_DIR      ${CMAKE_BINARY_DIR}/sdl2-image-2.0.5-src
        )

FetchContent_GetProperties(sdl2-image)
if(NOT sdl2-image_POPULATED)
    message(STATUS "Fetching sdl2-image 2.0.5")
    FetchContent_Populate(sdl2-image)
    message(STATUS "Fetched sdl2-image 2.0.5")
endif()

find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(TIFF REQUIRED)

add_library(sdl2-image-impl
        ${sdl2-image_SOURCE_DIR}/IMG.c
        ${sdl2-image_SOURCE_DIR}/IMG_bmp.c
        ${sdl2-image_SOURCE_DIR}/IMG_png.c
        ${sdl2-image_SOURCE_DIR}/IMG_jpg.c
        ${sdl2-image_SOURCE_DIR}/IMG_gif.c
        ${sdl2-image_SOURCE_DIR}/IMG_tga.c
        ${sdl2-image_SOURCE_DIR}/IMG_tif.c
        ${sdl2-image_SOURCE_DIR}/IMG_svg.c
        ${sdl2-image_SOURCE_DIR}/IMG_webp.c
        ${sdl2-image_SOURCE_DIR}/IMG_xxx.c
        ${sdl2-image_SOURCE_DIR}/IMG_xv.c
        ${sdl2-image_SOURCE_DIR}/IMG_xpm.c
        ${sdl2-image_SOURCE_DIR}/IMG_xcf.c
        ${sdl2-image_SOURCE_DIR}/IMG_WIC.c
        ${sdl2-image_SOURCE_DIR}/IMG_pnm.c
        ${sdl2-image_SOURCE_DIR}/IMG_pcx.c
        ${sdl2-image_SOURCE_DIR}/IMG_lbm.c
        ${sdl2-image_SOURCE_DIR}/SDL_image.h)
target_link_libraries(sdl2-image-impl PRIVATE ${JPEG_LIBRARIES} ${PNG_LIBRARIES} ${TIFF_LIBRARIES} SDL2)
target_include_directories(sdl2-image-impl PRIVATE ${JPEG_INCLUDE_DIRS} ${PNG_INCLUDE_DIRS} ${TIFF_INCLUDE_DIRS} ${sdl2_SOURCE_DIR}/include)
target_compile_definitions(sdl2-image-impl PRIVATE LOAD_BMP LOAD_PNG LOAD_JPEG LOAD_GIF LOAD_TGA LOAD_TIF LOAD_SVG)

add_library(sdl2-image-lib INTERFACE)
target_include_directories(sdl2-image-lib INTERFACE ${sdl2-image_SOURCE_DIR})
target_link_libraries(sdl2-image-lib INTERFACE sdl2-image-impl)
