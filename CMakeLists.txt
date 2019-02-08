cmake_minimum_required(VERSION 3.0)

set(LDFLAGS -fuse-ld=lld)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER clang-cl)
set(CMAKE_CXX_COMPILER clang-cl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /Ot /EHsc")

project(SimpleArray)

add_executable(${PROJECT_NAME} main.cpp)