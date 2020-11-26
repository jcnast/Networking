# CMakeList.txt : CMake project for Networking, include source and define
# project specific logic here.

# cmake version
cmake_minimum_required (VERSION 3.8)

set(EXEC networking)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(Networking)