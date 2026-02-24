cmake_minimum_required(VERSION 4.0)
project(Window LANGUAGES CXX)

# Configure library and dependencies
add_library(Window OBJECT)
target_sources(Window
	PRIVATE
	src/Window.cpp

	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS include/
	FILES
	include/Window/Window.hpp
)

target_include_directories(Window
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link dependent libraries
target_link_libraries(Window PRIVATE Event)

# Enable Testing
if(BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()
