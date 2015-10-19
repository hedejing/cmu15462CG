if(WIN32)
	set(CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR}/../windows)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
	add_definitions(-Wall -Wextra -pedantic)
endif()

find_package(SDL REQUIRED)
find_package(PNG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})
find_package(GLEW)
if (NOT GLEW_FOUND)
	add_subdirectory(glew)
endif()
