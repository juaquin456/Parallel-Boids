cmake_minimum_required(VERSION 3.24)
project(Parallel_Boids)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(ext/quadtree)



find_package(OpenMP REQUIRED)

add_executable(Parallel_Boids src/main.cpp
        src/Boid.h
        src/Helpers.h
        src/Benchmark.h)
target_link_libraries(Parallel_Boids PUBLIC OpenMP::OpenMP_CXX)
target_link_libraries(Parallel_Boids PRIVATE quadtree)
target_include_directories(Parallel_Boids PRIVATE ${CMAKE_SOURCE_DIR}/ext/quadtree/include)
target_include_directories(Parallel_Boids PRIVATE ${CMAKE_SOURCE_DIR}/ext/linalg)
