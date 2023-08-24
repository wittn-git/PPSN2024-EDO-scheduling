cmake_minimum_required(VERSION 3.5)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
project(Bachelor_Thesis VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

add_executable(Bachelor_Thesis 
                ${CMAKE_SOURCE_DIR}/src/main.cpp
            )   

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(Bachelor_Thesis PUBLIC OpenMP::OpenMP_CXX)
endif()

target_compile_features(Bachelor_Thesis PUBLIC cxx_std_17)
target_include_directories(Bachelor_Thesis PRIVATE include)