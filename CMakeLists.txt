cmake_minimum_required(VERSION 3.14)
project(newton-fractal)


set(SOURCES src/main.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 11)
target_link_libraries(${PROJECT_NAME} PUBLIC raylib)
