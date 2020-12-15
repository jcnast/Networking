# CMakeList.txt : CMake project for Networking, include source and define
# project specific logic here.

add_subdirectory(Logging)

add_library(Networking_Lib #
Client.cpp #
Server.cpp #
Network/Buffer.cpp #
Network/ByteFormatter.cpp #
Network/ByteParser.cpp #
Network/ClientConnection.cpp #
Network/Endpoint.cpp #
Network/Message.cpp #
Network/ServerConnection.cpp #
Network/Socket.cpp #
Network/Linux/LinuxSocket.cpp #
Network/Windows/WindowsSocket.cpp)
target_include_directories(Networking_Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Networking_Lib wsock32 ws2_32)

add_definitions(-D__WIN32__)

add_executable(Client Networking.cpp)
set_target_properties(Client PROPERTIES COMPILE_DEFINITIONS CLIENT)
target_link_libraries(Client Networking_Lib Logging_Lib)

add_executable(Server Networking.cpp)
set_target_properties(Server PROPERTIES COMPILE_DEFINITIONS SERVER)
target_link_libraries(Server Networking_Lib Logging_Lib)