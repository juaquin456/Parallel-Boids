cmake_minimum_required(VERSION 3.28)
project(Parallel_Boids)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(ext/quadtree)

set(SFML_BUILD_SHARED_LIBS TRUE)
add_subdirectory(${CMAKE_SOURCE_DIR}/ext/sfml)
include_directories(${SFML_INCLUDE_DIR})

find_package(OpenMP REQUIRED)

add_executable(Parallel_Boids src/main.cpp
        src/Boid.h
        src/Helpers.h
        src/Benchmark.h)
target_link_libraries(Parallel_Boids PUBLIC OpenMP::OpenMP_CXX)
target_link_libraries(Parallel_Boids PRIVATE quadtree)
target_include_directories(Parallel_Boids PRIVATE ${CMAKE_SOURCE_DIR}/ext/quadtree/include)
target_include_directories(Parallel_Boids PRIVATE ${CMAKE_SOURCE_DIR}/ext/linalg)
target_link_libraries(Parallel_Boids PRIVATE sfml-graphics sfml-window sfml-system)