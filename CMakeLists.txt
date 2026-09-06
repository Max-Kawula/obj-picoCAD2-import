cmake_minimum_required(VERSION 4.2)
project(ObjPicoCAD2Import)

set(CMAKE_C_STANDARD 99)

# for the gui platform and stuff.
#add_subdirectory(vendor/raylib)

#add_library(microui STATIC vendor/microui/src/microui.c)
#target_include_directories(microui PUBLIC vendor/microui/src)

add_library(cJSON STATIC vendor/cJSON/cJSON.c)
target_include_directories(cJSON PUBLIC vendor/cJSON)

add_library(tinyobj_loader_c INTERFACE)
target_include_directories(tinyobj_loader_c INTERFACE vendor/tinyobj_loader_c)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE vendor/stb)

#find_package(imagequant REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.c
    src/pico.c
    src/pico_image.c
    src/obj.c
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
target_link_libraries(${PROJECT_NAME} 
    cJSON 
    tinyobj_loader_c
    stb
    m
    imagequant
)
