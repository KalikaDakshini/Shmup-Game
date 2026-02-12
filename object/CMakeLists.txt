cmake_minimum_required(VERSION 4.0)
project(object LANGUAGES CXX)

# Configure library and dependencies
add_library(object OBJECT)
target_sources(object
	PRIVATE
	src/object.cpp

	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS include/
	FILES
	include/object.hpp
)

# Enable Testing
if(BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()
