cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(OpenGL REQUIRED)

set(SHOOTER_SOURCE ${PROJECT_SOURCE_DIR}/src/glad.c
			       ${PROJECT_SOURCE_DIR}/src/Application.cpp)

set(SHOOTER_HEADERS ${PROJECT_SOURCE_DIR}/src/glad.h
				    ${PROJECT_SOURCE_DIR}/src/khrplatform.h
				    ${PROJECT_SOURCE_DIR}/src/Application.h)

add_executable(ArenaShooter ${SHOOTER_HEADERS} ${SHOOTER_SOURCE})				

set_target_properties( ArenaShooter
    				   PROPERTIES
    				   ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib"
    				   LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib"
    				   RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin" )

if (WIN32)
    set_target_properties(ArenaShooter PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
endif()

target_link_libraries(ArenaShooter ${OPENGL_LIBRARIES})
target_link_libraries(ArenaShooter SDL2main)
target_link_libraries(ArenaShooter SDL2-static)