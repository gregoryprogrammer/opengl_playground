cmake_minimum_required(VERSION 3.7)
project(opengl_playground CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall \
                                                    -Wextra \
                                                    -Wno-switch \
                                                    -pedantic \
                                                    -Wno-gnu-statement-expression \
                                                    -O0 \
                                                    -ggdb \
                                                    -fno-omit-frame-pointer \
                                                    -fno-inline \
                                                    -fdiagnostics-color=always \
                                                    -fsanitize=address \
                                                    ")

                                                    # -fno-rtti \
                                                    # -fno-exceptions \

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} \
                                                        ")
                                                        # -fno-rtti \
                                                        # -fno-exceptions \

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/cmake")

include_directories("${CMAKE_SOURCE_DIR}/libs/")

message("CMake module path: " ${CMAKE_MODULE_PATH})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
message("Including OpenGL: " ${OPENGL_INCLUDE_DIR})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
message("Including GLEW: " ${GLEW_INCLUDE_DIRS})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIR})
message("Including GLM: " ${GLM_INCLUDE_DIR})

find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})
message("Including FreeType: " ${FREETYPE_INCLUDE_DIRS})

include_directories("${CMAKE_SOURCE_DIR}/src/")
file(GLOB_RECURSE GAME_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cc")

set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets/")
file(COPY ${ASSETS_DIR} DESTINATION ${CMAKE_BINARY_DIR}/assets/)

add_executable(${CMAKE_PROJECT_NAME} ${GAME_SOURCES})

target_link_libraries(${CMAKE_PROJECT_NAME} ${SDL2_LIBRARY})
target_link_libraries(${CMAKE_PROJECT_NAME} ${OPENGL_gl_LIBRARY})
target_link_libraries(${CMAKE_PROJECT_NAME} ${GLEW_LIBRARIES})
target_link_libraries(${CMAKE_PROJECT_NAME} ${FREETYPE_LIBRARIES})

add_subdirectory(tests)
