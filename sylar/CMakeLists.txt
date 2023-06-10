cmake_minimum_required(VERSION 3.0)

project(sylar)

set(LIB_SRC 
    src/log.cpp)

# add_subdirectory(src)

add_library(src SHARED ${LIB_SRC})
add_executable(main tests/main.cpp)
add_dependencies(main src)
target_link_libraries(main src)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)