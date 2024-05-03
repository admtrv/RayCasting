cmake_minimum_required(VERSION 3.5)
project(RayCastingEngine VERSION 0.1.0 LANGUAGES C)

add_executable(RayCastingEngine main.c)

include_directories("C:/MinGW/include") # Путь к заголовочным файлам pdcurses
link_directories("C:/MinGW/lib")        # Путь к библиотекам pdcurses

# Поиск и подключение библиотеки
find_library(PDCURSES_LIBRARY NAMES pdcurses)

if(PDCURSES_LIBRARY)
    message(STATUS "Library pdcurses found: ${PDCURSES_LIBRARY}")
    target_link_libraries(RayCastingEngine ${PDCURSES_LIBRARY})
else()
    message(FATAL_ERROR "Error finding pdcurses library")
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
