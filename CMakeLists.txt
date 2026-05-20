cmake_minimum_required(VERSION 3.10)

# Project name
project(MovieBookingService)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add executable
add_executable(movie_booking_app
    src/main.cpp
    src/BookingService.cpp
)

add_library(booking
    src/BookingService.cpp
)

# Link include directories to target
target_include_directories(movie_booking_app PRIVATE ${CMAKE_SOURCE_DIR}/include)

option(BUILD_TESTING "Build the tests" ON)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
