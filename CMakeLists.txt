cmake_minimum_required(VERSION 3.0)
project(chippy CXX)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp CPU.cpp CPU.h disassembler.cpp disassembler.h beeper.cpp beeper.h logo.h timer.cpp timer.h)
add_executable(chippy ${SOURCE_FILES})


find_package(SDL2 REQUIRED)

IF (WIN32)
include_directories(chippy ${SDL2_INCLUDE_DIRS})
target_link_libraries(chippy ${SDL2_LIBRARIES})
ELSE()
target_link_libraries(chippy SDL2::SDL2)
ENDIF()

option(WITH_CURSES "curses debug output" OFF)

if(MSVC)
  target_compile_options(chippy PRIVATE /W4)
else()
  target_compile_options(chippy PRIVATE -Wall -Wextra -pedantic)
endif()

if(WITH_CURSES)
    find_package(Curses REQUIRED)
    target_link_libraries(chippy ${CURSES_LIBRARIES})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_CURSES=")   
endif()







