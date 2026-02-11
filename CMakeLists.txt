cmake_minimum_required(VERSION 4.0)
project("CPP Project" LANGUAGES CXX)

# Set variables
set(MAIN_TARGET "renderer")
set(BIN_DIR "./bin")
set(TST_DIR "./tests")

# Main executable
add_executable(${MAIN_TARGET} src/main.cpp)

# Compilation flags
set(BASE_FLAGS
	-Wall -Wextra -Wshadow -Wnon-virtual-dtor
	-Wold-style-cast -Wcast-align -Woverloaded-virtual
	-pedantic -Werror
)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_compile_options(${MAIN_TARGET} PRIVATE ${BASE_FLAGS})
target_compile_features(${MAIN_TARGET} PRIVATE cxx_std_20)

# Build specific options
target_compile_options(${MAIN_TARGET} PRIVATE
	$<$<CONFIG:Release>:-O3>
)
target_compile_options(${MAIN_TARGET} PRIVATE
	$<$<CONFIG:Debug>:-ggdb -O0>
)

# Append CMake path
list(APPEND CMAKE_MODULE_PATH "/usr/lib/cmake")

# Executable name
set_target_properties(${MAIN_TARGET} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
	OUTPUT_NAME_DEBUG "tranny.dbg"
	OUTPUT_NAME_RELEASE "tranny.app"
)

# Testing
option(BUILD_TESTING "Enable testing and build tests" ON)

# Link libraries
find_package(SFML 3 REQUIRED Graphics System Window)
target_link_libraries(${MAIN_TARGET} PRIVATE SFML::Graphics)
