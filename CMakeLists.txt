cmake_minimum_required(VERSION 3.29)
project(Parallel_Boids)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(ext/quadtree)

set(SFML_BUILD_SHARED_LIBS TRUE)
add_subdirectory(${CMAKE_SOURCE_DIR}/ext/sfml)
include_directories(${SFML_INCLUDE_DIR})

add_executable(Parallel_Boids src/main.cpp
        src/Boid.h)

target_link_libraries(Parallel_Boids PRIVATE quadtree)
target_include_directories(Parallel_Boids PRIVATE ${CMAKE_SOURCE_DIR}/ext/quadtree/include)
target_include_directories(Parallel_Boids PRIVATE ${CMAKE_SOURCE_DIR}/ext/linalg)
target_link_libraries(Parallel_Boids PRIVATE sfml-graphics sfml-window sfml-system)