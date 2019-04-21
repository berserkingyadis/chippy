cmake_minimum_required(VERSION 3.0)
project(chippy)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp CPU.cpp CPU.h disassembler.cpp disassembler.h beeper.cpp beeper.h logo.h timer.cpp timer.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall" )
add_executable(chippy ${SOURCE_FILES})

find_package(SDL2 REQUIRED)

target_link_libraries(chippy SDL2::SDL2)

option(WITH_CURSES "curses debug output" ON)

if(WITH_CURSES)
    find_package(Curses REQUIRED)
    target_link_libraries(chippy ${CURSES_LIBRARIES})
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_CURSES=" )
endif()






