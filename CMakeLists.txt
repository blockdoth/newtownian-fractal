cmake_minimum_required(VERSION 3.14)
project(newton-fractal)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -march=native -funroll-loops -fno-exceptions -fno-rtti -fomit-frame-pointer -g")


set(SOURCES 
  src/main.cpp
  src/fractal.cpp
)

set(ISPC_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/fractal.ispc)
set(ISPC_OBJECT ${CMAKE_CURRENT_BINARY_DIR}/fractal_ispc.o)
set(ISPC_HEADER ${CMAKE_CURRENT_BINARY_DIR}/fractal_ispc.h)

add_custom_command(
    OUTPUT ${ISPC_OBJECT} ${ISPC_HEADER}
    COMMAND ispc ${ISPC_SOURCE}
            -o ${ISPC_OBJECT}
            -h ${ISPC_HEADER}
            --target=avx2-i32x8
            --opt=fast-math
    DEPENDS ${ISPC_SOURCE}
)

add_executable(${PROJECT_NAME} ${SOURCES} ${ISPC_OBJECT})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
target_link_libraries(${PROJECT_NAME} PUBLIC raylib)
