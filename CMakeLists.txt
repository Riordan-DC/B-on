cmake_minimum_required(VERSION 3.0)
project(Beon)

find_package(OpenGL REQUIRED)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(Beon/libs/glfw)

option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(Beon/libs/assimp)

option(BUILD_BULLET2_DEMOS OFF)
option(BUILD_CPU_DEMOS OFF)
option(BUILD_EXTRAS OFF)
option(BUILD_OPENGL3_DEMOS OFF)
option(BUILD_UNIT_TESTS OFF)
add_subdirectory(Beon/libs/bullet)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

include_directories(Beon/headers/
                    ${CMAKE_CURRENT_BINARY_DIR}/Beon/libs/assimp/include/
                    ${CMAKE_CURRENT_SOURCE_DIR}/Beon/libs/assimp/include/
                    Beon/libs/bullet/src/
                    Beon/libs/glad/include/
                    Beon/libs/glfw/include/
                    Beon/libs/imgui/
                    Beon/libs/imgui/examples/
                    Beon/libs/glm/
                    Beon/libs/stb/
                    Beon/assets
                    Beon/shaders)

file(GLOB GLAD_SOURCES Beon/libs/glad/src/glad.c)
file(GLOB IMGUI_SOURCES Beon/libs/imgui/*.h
						Beon/libs/imgui/*.cpp
						Beon/libs/imgui/examples/imgui_impl_opengl3.h
						Beon/libs/imgui/examples/imgui_impl_opengl3.cpp
						Beon/libs/imgui/examples/imgui_impl_glfw.h
						Beon/libs/imgui/examples/imgui_impl_glfw.cpp)
file(GLOB PROJECT_HEADERS Beon/headers/*.hpp)
file(GLOB PROJECT_SOURCES Beon/src/*.cpp)
file(GLOB PROJECT_SHADERS Beon/shaders/*.comp
                          Beon/shaders/*.frag
                          Beon/shaders/*.geom
                          Beon/shaders/*.vert)
file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          Readme.md
                         .gitattributes
                         .gitignore
                         .gitmodules)

source_group("headers" FILES ${PROJECT_HEADERS})
source_group("shaders" FILES ${PROJECT_SHADERS})
source_group("src" FILES ${PROJECT_SOURCES})
source_group("glad" FILES ${GLAD_SOURCES})
source_group("imgui" FILES ${IMGUI_SOURCES})

file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/Beon/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Beon)

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                               ${PROJECT_SHADERS} ${PROJECT_CONFIGS}
                               ${GLAD_SOURCES} ${IMGUI_SOURCES})
target_link_libraries(${PROJECT_NAME} assimp glfw
                      ${GLFW_LIBRARIES} ${GLAD_LIBRARIES}
                      BulletDynamics BulletCollision LinearMath)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
