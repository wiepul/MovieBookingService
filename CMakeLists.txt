cmake_minimum_required(VERSION 3.14)

# Project name
project(MovieBookingService)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Library: booking

add_library(booking STATIC
    src/BookingService.cpp
)

target_include_directories(booking PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Executable: booking-cli

# Separate CLI executable that links the library
add_executable(movie_booking_cli
    src/main.cpp
)

target_link_libraries(movie_booking_cli PRIVATE booking)

# Tests (opt-in)

option(BUILD_TESTING "Build the tests" ON)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
