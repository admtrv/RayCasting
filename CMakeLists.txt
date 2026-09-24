# CMakeLists.txt

cmake_minimum_required(VERSION 3.22)
project(RayCasting C)

set(CMAKE_C_STANDARD 11)

# creating executable
add_executable(${PROJECT_NAME} main.c)

# linking curses library
find_package(Curses REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ${CURSES_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CURSES_INCLUDE_DIR})

# linking math library
target_link_libraries(${PROJECT_NAME} PRIVATE m)
