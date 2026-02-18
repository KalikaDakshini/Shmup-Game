cmake_minimum_required(VERSION 4.0)
project(object LANGUAGES CXX)

# Configure library and dependencies
add_library(object OBJECT)
target_sources(object
	PRIVATE
	src/Player.cpp
	src/Bullet.cpp
	src/Enemy.cpp

	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS include/
	FILES
	include/Object.hpp
	include/Behaviour.hpp
)

# Enable Testing
if(BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()
