cmake_minimum_required(VERSION 3.16)

project(hw_socket)

set(CMAKE_CXX_STANDARD 20)


add_executable(server server.cpp common.cpp Game.cpp)
add_executable(client client.cpp common.cpp Game.cpp)
target_link_libraries(server pthread)
target_link_libraries(client pthread)


#set(Boost_USE_STATIC_LIBS OFF)
#set(Boost_USE_MULTITHREADED ON)
#set(Boost_USE_STATIC_RUNTIME OFF)
#find_package(Boost COMPONENTS system)
#if(Boost_FOUND)
#    include_directories(${Boost_INCLUDE_DIRS})
#    target_link_libraries(server ${Boost_LIBRARIES})
#endif()