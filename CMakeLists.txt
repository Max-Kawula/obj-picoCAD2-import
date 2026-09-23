cmake_minimum_required(VERSION 4.2)
project(ObjPicoCAD2Import)

set(CMAKE_C_STANDARD 99)

add_subdirectory(vendor/raylib-6.0)


add_library(cJSON STATIC vendor/cJSON/cJSON.c)
target_include_directories(cJSON PUBLIC vendor/cJSON)

add_library(tinyobj_loader_c INTERFACE)
target_include_directories(tinyobj_loader_c INTERFACE vendor/tinyobj_loader_c)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE vendor/stb)

add_library(microui STATIC vendor/microui/src/microui.c)
target_include_directories(microui PUBLIC vendor/microui/src)

# Check for libimagequant for image loading support:
find_library(IMAGEQUANT_LIB imagequant)
find_path(IMAGEQUANT_INCLUDE libimagequant.h)

add_executable(${PROJECT_NAME}
    src/main.c
    src/pico.c
    src/pico_image.c
    src/obj.c
    src/gui.c
    src/dialogue.c
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)

if(IMAGEQUANT_LIB AND IMAGEQUANT_INCLUDE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_IMAGEQUANT)
    target_include_directories(${PROJECT_NAME} PRIVATE ${IMAGEQUANT_INCLUDE})
    target_link_libraries(${PROJECT_NAME} ${IMAGEQUANT_LIB})
    message(STATUS "libimagequant found: ENABLED")
else()
    message(STATUS "libimagequant not found: DISABLED")
endif()

target_link_libraries(${PROJECT_NAME} 
    raylib
    cJSON 
    tinyobj_loader_c
    stb
    microui
    m
)
