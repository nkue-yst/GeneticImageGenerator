cmake_minimum_required(VERSION 3.10)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(GeneticImageGenerator CXX)
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_executable(GeneticImageGenerator ${SOURCES})

if(WIN32)
	set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
	set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT GeneticImageGenerator)
endif(WIN32)

### Setup SDL2 ###
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(SDL)
set(SDL2_DIR ${CMAKE_BINARY_DIR}/SDL)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(GeneticImageGenerator ${SDL2_STATIC_LIBRARIES})

### Setup SDL_Image ###
add_subdirectory(SDL_image)
set(SDL2_image_DIR ${CMAKE_BINARY_DIR}/SDL_image)
find_package(SDL2_image REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/SDL_image)
target_link_libraries(GeneticImageGenerator SDL2_image::SDL2_image-static)
