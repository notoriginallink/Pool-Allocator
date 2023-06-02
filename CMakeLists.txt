CMAKE_MINIMUM_REQUIRED(VERSION 3.24)
project(labwork-8-notoriginallink VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# add lib
add_subdirectory(lib/PoolAllocator)

# Add executable
ADD_EXECUTABLE(
        Allocator
        main.cpp
        lib/PoolAllocator/PoolAllocator.cpp
)

ADD_EXECUTABLE(
        bench
        tests/benchmark.cpp
        lib/PoolAllocator/PoolAllocator.cpp
)

target_link_libraries(Allocator PRIVATE PoolAllocator)


