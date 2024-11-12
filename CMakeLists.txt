cmake_minimum_required(VERSION 3.29)
project(Parallel_Boids)

set(CMAKE_CXX_STANDARD 20)
add_subdirectory(ext/quadtree)
add_executable(Parallel_Boids src/main.cpp
        src/Boid.h)

target_link_libraries(Parallel_Boids PRIVATE quadtree)
target_include_directories(Parallel_Boids PRIVATE ${CMAKE_SOURCE_DIR}/ext/quadtree/include)
target_include_directories(Parallel_Boids PRIVATE ${CMAKE_SOURCE_DIR}/ext/linalg)
