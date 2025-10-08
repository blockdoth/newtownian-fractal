cmake_minimum_required(VERSION 3.14)
project(newton-fractal)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -march=native -funroll-loops -fno-exceptions -fno-rtti -fomit-frame-pointer -g")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

set(SOURCES src/main.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 11)
target_link_libraries(${PROJECT_NAME} PUBLIC raylib)
