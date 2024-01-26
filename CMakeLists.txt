cmake_minimum_required(VERSION 3.20)
project(GameEngine)
option(ENGINE_BUILD_SANDBOXES "Build developer sandboxes" ON )
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory("ext/libsndfile-master")
include_directories("include")
include_directories("ext/STB")
include_directories("ext/libsndfile-master/include")

# enet-1.3.17 library
add_subdirectory("ext/enet-1.3.17")
include_directories("ext/enet-1.3.17/include")

# freetype-2.13.1 library
add_subdirectory("ext/freetype-2.13.1")
include_directories("ext/freetype-2.13.1/include")

# tmxlite-master library
SET(TMXLITE_STATIC_LIB TRUE CACHE BOOL "")
add_subdirectory("ext/tmxlite-master/tmxlite")

# miniaud.io
include_directories("ext/audio")

# Assimp model loading library
option(ASSIMP_BUILD_TESTS OFF)
option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
add_subdirectory("ext/assimp-5.0.1")

# OpenGL
add_subdirectory("ext/glfw-3.3.2")
add_subdirectory("ext/glm-0.9.9.7")
include_directories("${GLFW_SOURCE_DIR}/deps")
set(GLAD_GL "${GLFW_SOURCE_DIR}/deps/glad/gl.h" "${GLFW_SOURCE_DIR}/deps/glad_gl.c")

file(GLOB_RECURSE ENGINE_SOURCE_FILES "./src/*.cpp")
file(GLOB_RECURSE ENGINE_HEADER_FILES "./include/engine/*.h" "./include/engine/GL/*.h")

# engine -library
add_library(engine ${ENGINE_SOURCE_FILES} ${ENGINE_HEADER_FILES} ${GLAD_GL} ${STB})

IF (WIN32)
	target_link_libraries(engine glfw tmxlite glm sndfile freetype assimp enet winmm ws2_32)
ELSE()
	target_link_libraries(engine glfw tmxlite glm sndfile freetype assimp enet)
ENDIF()


target_include_directories(engine PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# Sandboxes
if(ENGINE_BUILD_SANDBOXES)
	add_subdirectory("sandboxes")
	# Copy Assets
	file(COPY "./sandboxes/sandbox1/assets" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/sandboxes/sandbox1)
	file(COPY "./sandboxes/sandbox2/assets" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/sandboxes/sandbox2)
	file(COPY "./sandboxes/sandbox3/assets" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/sandboxes/sandbox3)
	file(COPY "./sandboxes/sandbox4/assets" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/sandboxes/sandbox4)
	file(COPY "./sandboxes/sandbox5/assets" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/sandboxes/sandbox5)
	file(COPY "./sandboxes/sandbox6/assets" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/sandboxes/sandbox6)
	file(COPY "./sandboxes/sandbox7/assets" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/sandboxes/sandbox7)
	file(COPY "./sandboxes/TORPEDOES/assets" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/sandboxes/TORPEDOES)
endif()
