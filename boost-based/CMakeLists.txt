cmake_minimum_required(VERSION 3.10)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 17)
link_libraries(pthread boost_thread boost_system)
add_executable(ThreadPool main.cpp ThreadPool.cpp ThreadPool.h)