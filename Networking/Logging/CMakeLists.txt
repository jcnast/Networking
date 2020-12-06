# CMakeList.txt : CMake project for Networking, include source and define
# project specific logic here.

add_library(Logging_Lib #
Logger.cpp #
ConsoleLogger.cpp)
target_include_directories(Logging_Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})