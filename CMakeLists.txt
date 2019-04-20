cmake_minimum_required(VERSION 3.0)
project(chippy)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp CPU.cpp CPU.h disassembler.cpp disassembler.h beeper.cpp beeper.h logo.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall" )
add_executable(chippy ${SOURCE_FILES})

find_package(SDL2 REQUIRED)

target_link_libraries(chippy SDL2::SDL2)

option(WITHOUT_CURSES "exclude curses debug output" ON)

if(WITHOUT_CURSES)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITHOUT_CURSES=" )
else()
    find_package(Curses REQUIRED)
    target_link_libraries(chippy ${CURSES_LIBRARIES})
endif()






