cmake_minimum_required(VERSION 3.0.0)
project(RayCastingEngine VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

add_executable(RayCastingEngine main.c)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
target_link_libraries(RayCastingEngine ${CURSES_LIBRARIES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
