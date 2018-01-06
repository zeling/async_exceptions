cmake_minimum_required(VERSION 3.5)
project(async_exception)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-pthread")

add_executable(async_exception main.cpp posix.cpp actor.hh unbounded_mutex_protected_queue.hh job_actor.hh)
