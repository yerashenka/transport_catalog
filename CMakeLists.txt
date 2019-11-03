cmake_minimum_required(VERSION 3.10)
project(Transport_Catalog)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC)
add_definitions(-D_LIBCPP_DEBUG=1)
set(CMAKE_CXX_FLAGS "-fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls -g")

add_subdirectory(src)
add_subdirectory(test)