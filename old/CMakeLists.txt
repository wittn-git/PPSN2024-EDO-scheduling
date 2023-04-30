project(BA)

cmake_minimum_required(VERSION 3.22)
if(POLICY CMP0012)
  cmake_policy(SET CMP0012 NEW)
endif()

find_package(Pagmo REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main Pagmo::pagmo)