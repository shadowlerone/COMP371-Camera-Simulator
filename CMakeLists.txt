cmake_minimum_required (VERSION 3.8)

project(camera) # The name of your choice for the project comes here
set(CMAKE_BUILD_TYPE Debug)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
	-DGLM_ENABLE_EXPERIMENTAL
	-DUSE_ZLIB_T
)

IF(WIN32)

# set stuff for windows
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

# for windows
include_directories(C:/glfw-3.3.9.bin.WIN64/include/)
include_directories(C:/glew-2.1.0-src/include/)
include_directories(C:/glew-2.1.0-src/include/GL/)
include_directories(C:/GLM_0.9.9.8/g-truc-glm-bf71a83/)

link_directories(C:/glew-2.1.0/lib/Release/x64/)
link_directories(C:/glfw-3.3.9.bin.WIN64/lib-vc2019/)

set(ALL_LIBS
	${OPENGL_gl_LIBRARY}
	OpenGL::GLU 
	glew32s 
	glfw3
)

ELSE()

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_STANDARD 17) #optional to use c++11

set(CMAKE_PREFIX_PATH
    /encs # For ENCS lab computers
    /opt/local # Macports
	./assimp
)

find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(GLEW REQUIRED)
# find_library(assimp REQUIRED)
# find_program(assimp REQUIRED)
find_package(glfw3 REQUIRED
    HINTS /encs/pkg/glfw-3.3.4/root # ENCS installation of glfw
)

set(ALL_LIBS
	OpenGL::GL
	OpenGL::GLU
	GLEW::glew
	glfw
	# assimp
)

ENDIF()

# NOTE: ENCS glm installation is missing links to *.inl files so we need this line
include_directories(/encs/pkg/glm-0.9.9.8/root/include)


# include_directories(assimp/include/assimp)


AUX_SOURCE_DIRECTORY(src SOURCES)
AUX_SOURCE_DIRECTORY(src/learnopengl SOURCES)
AUX_SOURCE_DIRECTORY(imgui SOURCES)
# AUX_SOURCE_DIRECTORY(src/assimp SOURCES)
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${ALL_LIBS})

target_include_directories(${PROJECT_NAME} PRIVATE assimp/include/assimp)

target_link_libraries(${PROJECT_NAME} assimp)

include_directories(src)
INCLUDE_DIRECTORIES(imgui)


add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/assets/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets/)
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_directory
						${CMAKE_SOURCE_DIR}/shaders/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/)

