cmake_minimum_required(VERSION 4.0)
project(Object LANGUAGES CXX)

# Configure library and dependencies
add_library(Object OBJECT)
target_sources(Object
	PRIVATE
	src/Player.cpp
	src/ObjBase.cpp
	src/FireMode.cpp
	src/Bullet.cpp
	src/World.cpp

	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS include/
	FILES

	include/Object/ObjBase.hpp
	include/Object/Bullet.hpp
	include/Object/Player.hpp
	include/Object/World.hpp
)

target_include_directories(Object
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Object PRIVATE Event)

# Enable Testing
if(BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()
