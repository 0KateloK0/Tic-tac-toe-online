cmake_minimum_required(VERSION 3.23)
project(hw_socket)

set(CMAKE_CXX_STANDARD 20)


add_executable(server server.cpp common.cpp Game.cpp)
add_executable(client client.cpp common.cpp Game.cpp)
target_link_libraries(server pthread)
target_link_libraries(client pthread)

