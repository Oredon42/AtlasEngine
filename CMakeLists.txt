set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

project(AtlasEngine)

cmake_minimum_required(VERSION 2.8)

# Checking submodules
if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/openmesh")
  message(FATAL_ERROR "The OpenMesh submodule directory is missing, you have to clone the project with --recursive")
endif()

if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp")
  message(FATAL_ERROR "The Assimp submodule directory is missing, you have to clone the project with --recursive")
endif()

# Get sources and headers
set(SOURCE_DIR src)
set(INCLUDE_DIR include)

file(GLOB_RECURSE SOURCES ${SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${INCLUDE_DIR}/*.h)

set (INCLUDE_DIRS "")
foreach (_headerFile ${HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES INCLUDE_DIRS)

include_directories(${INCLUDE_DIRS})

# Add submodules
add_subdirectory(lib/openmesh)
include_directories(lib/openmesh/src)
add_subdirectory(lib/assimp)
include_directories(lib/assimp/include)
add_subdirectory(lib/glm)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(APPLE)
	set(CMAKE_CXX_FLAGS "-std=c++11 -g -ggdb -O0 -Wall -Wextra")
	#OpenGL inclusion
	find_package(OpenGL)
	include_directories(${OPENGL_INCLUDE_DIR})

	#Qt5 inclusion
	find_package(Qt5Core)
	find_package(Qt5Gui)
	find_package(Qt5Widgets)
	find_package(Qt5OpenGL)
        include_directories(${Qt5Widgets_INCLUDE_DIRS})
	add_definitions(${Qt5Widgets_DEFINITIONS})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

        #ASSIMP inclusion
        include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp)
        link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp)

        #OpenMesh inclusion
        include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/openmesh)
        link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/openmesh)

        add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

	qt5_use_modules(${PROJECT_NAME} Core Gui Widgets OpenGL)

        target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES} ${OPENGL_LIBRARIES} libassimpd.dylib libOpenMeshCore.dylib)
elseif(UNIX)
        set(CMAKE_CXX_FLAGS "-std=c++11 -g -ggdb -O0 -Wall -Wextra")
        find_package(OpenGL REQUIRED)
        find_package(Qt4 REQUIRED)
        set(Qt4_LIBRARIES Qt4::QtGui Qt4::QtCore Qt4::QtOpenGL)

        #ASSIMP inclusion
        include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp)
        link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp)

        #OpenGL inclusion
        include_directories(${OPENGL_INCLUDE_DIR})

        include_directories(${Qt5Widgets_INCLUDE_DIRS})

        add_executable(${PROJECT_NAME} ${SRC_LIST})

        target_link_libraries(${PROJECT_NAME} ${Qt4_LIBRARIES} ${OPENGL_LIBRARIES} libassimp.so)
elseif(WIN32)
        add_definitions(-D_USE_MATH_DEFINES)

        set(CMAKE_CXX_FLAGS "")
    	#OpenGL inclusion
    	find_package(OpenGL)
    	include_directories(${OPENGL_INCLUDE_DIR})

    	#Qt5 inclusion
    	find_package(Qt5Core)
    	find_package(Qt5Gui)
    	find_package(Qt5Widgets)
    	find_package(Qt5OpenGL)
    	include_directories(${Qt5Widgets_INCLUDE_DIRS})
    	add_definitions(${Qt5Widgets_DEFINITIONS})
    	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

    	#GLEW inclusion
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/glew)

        link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/glew
            ${PROJECT_BINARY_DIR}/lib/openmesh
            ${PROJECT_BINARY_DIR}/lib/assimp)

    	add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

    	qt5_use_modules(${PROJECT_NAME} Core Gui Widgets OpenGL)

        target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES} ${OPENGL_LIBRARIES}
            ${PROJECT_BINARY_DIR}/lib/glew/glew32.lib
            ${PROJECT_BINARY_DIR}/lib/openmesh/src/OpenMesh/Core/OpenMeshCored.lib
            ${PROJECT_BINARY_DIR}/lib/openmesh/src/OpenMesh/Tools/OpenMeshToolsd.lib
            ${PROJECT_BINARY_DIR}/lib/assimp/code/assimp-vc140-mtd.lib )

        configure_file(${PROJECT_BINARY_DIR}/lib/glew/glew32.dll ${PROJECT_BINARY_DIR}/glew32.dll COPYONLY)
        configure_file(${PROJECT_BINARY_DIR}/lib/assimp/code/assimp-vc140-mtd.dll ${PROJECT_BINARY_DIR}/assimp-vc140-mtd.dll COPYONLY)
endif()
