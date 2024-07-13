cmake_minimum_required(VERSION 3.5)
project(RayCasting VERSION 0.1.0 LANGUAGES C)

add_executable(RayCasting main.c)

# Searching and connecting library
find_library(PDCURSES_LIBRARY NAMES pdcurses PATHS "C:/MinGW/lib")

if(PDCURSES_LIBRARY)
    message(STATUS "Library pdcurses found: ${PDCURSES_LIBRARY}")
    target_link_libraries(RayCasting ${PDCURSES_LIBRARY})
else()
    message(FATAL_ERROR "Error finding pdcurses library")
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
