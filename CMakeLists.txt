set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

project(AtlasEngine)

cmake_minimum_required(VERSION 2.8)

# Checking submodules
if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp")
  message(FATAL_ERROR "The Assimp submodule directory is missing, you need to clone the project with --recursive")
endif()

if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/glm")
  message(FATAL_ERROR "The GLM submodule directory is missing, you need to clone the project with --recursive")
endif()

if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/openmesh")
  message(FATAL_ERROR "The OpenMesh submodule directory is missing, you need to clone the project with --recursive")
endif()

if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/sfml")
  message(FATAL_ERROR "The SFML submodule directory is missing, you need to clone the project with --recursive")
endif()

# Set sources dir
set(SOURCE_DIR src)

# Look for sources files
file(GLOB_RECURSE SOURCES_LIST ${SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS_LIST ${SOURCE_DIR}/*.h)

# Recursive search of headers files
set (INCLUDE_DIRS "")
foreach (_headerFile ${HEADERS_LIST})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND SOURCE_DIR ${_dir})
endforeach()
list(REMOVE_DUPLICATES INCLUDE_DIRS)

include_directories(${SOURCE_DIR})

# Add submodules
add_subdirectory(lib/assimp)
add_subdirectory(lib/openmesh)
add_subdirectory(lib/sfml)
add_subdirectory(lib/glew)
include_directories(lib/assimp/include/assimp)
include_directories(lib/openmesh/src)
include_directories(lib/glm/glm)
include_directories(lib/sfml/include/SFML)
include_directories(lib/glew/include/GL)

# Copy shader folder
file(COPY shaders DESTINATION ${PROJECT_BINARY_DIR})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_FLAGS "-std=c++11 -g -ggdb -O0 -Wall -Wextra")

#OpenGL inclusion
find_package(OpenGL)
include_directories(${OPENGL_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

add_executable(${PROJECT_NAME} ${SOURCES_LIST} ${HEADERS_LIST})

target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES} ${OPENGL_LIBRARIES}
	OpenMeshCore
	OpenMeshTools
  assimp)

