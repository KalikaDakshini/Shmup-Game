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

	# src/Bullet.cpp
	# src/Enemy.cpp
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS include/
	FILES

	include/Object/Bullet.hpp
	include/Object/Behaviour.hpp
	include/Object/Player.hpp

	# include/Object/Enemy.hpp
)

target_include_directories(Object
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(Object PUBLIC Event)

# Enable Testing
if(BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()
