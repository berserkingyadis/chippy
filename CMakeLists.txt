cmake_minimum_required(VERSION 3.0)
project(chippy)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp CPU.cpp CPU.h disassembler.cpp disassembler.h beeper.cpp beeper.h logo.h timer.cpp timer.h)


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

conan_basic_setup()

add_executable(chippy ${SOURCE_FILES})
target_link_libraries(chippy ${CONAN_LIBS})
option(WITH_CURSES "curses debug output" OFF)

if(WITH_CURSES)
    find_package(Curses REQUIRED)
    target_link_libraries(chippy ${CURSES_LIBRARIES})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_CURSES=1")   
endif()







