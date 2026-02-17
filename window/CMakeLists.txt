cmake_minimum_required(VERSION 4.0)
project(Window LANGUAGES CXX)

# Configure library and dependencies
add_library(window OBJECT)
target_sources(window
PRIVATE
src/Window.cpp

PUBLIC
FILE_SET HEADERS
BASE_DIRS include/
FILES
include/Window.hpp
)

# Enable Testing
if(BUILD_TESTS)
enable_testing()
add_subdirectory(tests)
endif()
