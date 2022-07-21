cmake_minimum_required(VERSION 3.22)
project(socket_simple)

set(CMAKE_CXX_STANDARD 14)

add_executable(socket_simple server.cpp client.cpp Serverr.cpp Serverr.h communicator.cpp Clients.cpp Clients.h)
