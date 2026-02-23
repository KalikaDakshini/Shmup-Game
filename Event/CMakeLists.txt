cmake_minimum_required(VERSION 4.0)
project(Event LANGUAGES CXX)

# Configure library and dependencies
add_library(Event INTERFACE)
target_sources(Event
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS include/
	FILES
	include/Event/GameEvent.hpp
)
