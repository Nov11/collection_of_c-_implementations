cmake_minimum_required(VERSION 3.5)
project(threadpool)

set(CMAKE_CXX_STANDARD 11)
find_package (Threads)
add_executable(threadpool main.cpp)
target_link_libraries (threadpool ${CMAKE_THREAD_LIBS_INIT})