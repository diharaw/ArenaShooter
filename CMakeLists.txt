cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project("ArenaShooter")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(SDL_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/external/SDL2/include")
set(GLM_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/external/glm/glm")
set(STB_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/external/stb")
set(JSON_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/external/json/single_include/nlohmann")

include_directories("${GLM_INCLUDE_DIRS}"
                    "${STB_INCLUDE_DIRS}"
                    "${JSON_INCLUDE_DIRS}"
                    "${SDL_INCLUDE_DIRS}")

add_subdirectory(external/SDL2)
add_subdirectory(src)