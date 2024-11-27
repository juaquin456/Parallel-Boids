cmake_minimum_required(VERSION 3.28)
project(Parallel_Boids)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(ext/quadtree)
file(GLOB IMGUI_SOURCES
        ${CMAKE_SOURCE_DIR}/ext/imgui/*.cpp
        ${CMAKE_SOURCE_DIR}/ext/imgui/backends/imgui_impl_sdl2.cpp
        ${CMAKE_SOURCE_DIR}/ext/imgui/backends/imgui_impl_sdlrenderer2.cpp
)

file(GLOB IMGUI_HEADERS
        ${CMAKE_SOURCE_DIR}/ext/imgui/*.h
        ${CMAKE_SOURCE_DIR}/ext/imgui/backends/imgui_impl_sdl2.h
        ${CMAKE_SOURCE_DIR}/ext/imgui/backends/imgui_impl_sdlrenderer2.h
)
add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS})
target_include_directories(imgui PUBLIC
        ${CMAKE_SOURCE_DIR}/ext/imgui
        ${CMAKE_SOURCE_DIR}/ext/imgui/backends
)
find_package(OpenMP REQUIRED)
find_package(SDL2 REQUIRED)
target_link_libraries(imgui PUBLIC SDL2::SDL2 SDL2::SDL2main)
add_executable(Parallel_Boids src/main.cpp
        src/Boid.h
        src/Constants.h)
target_link_libraries(Parallel_Boids PRIVATE imgui SDL2::SDL2 SDL2::SDL2main)
target_link_libraries(Parallel_Boids PUBLIC OpenMP::OpenMP_CXX)
target_link_libraries(Parallel_Boids PRIVATE quadtree)
target_include_directories(Parallel_Boids PRIVATE ${CMAKE_SOURCE_DIR}/ext/quadtree/include)
target_include_directories(Parallel_Boids PRIVATE ${CMAKE_SOURCE_DIR}/ext/linalg)