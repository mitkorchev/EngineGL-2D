cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE "/home/dimkorchev/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(VCPKG_TARGET_TRIPLET "x64-linux")

project(EngineGL2D LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)

add_executable(Engine
    src/main.cpp
)

target_link_libraries(Engine PRIVATE
    glfw
    GLEW::GLEW
)

if (WIN32)
    target_link_libraries(Engine PRIVATE opengl32)
elseif(UNIX AND NOT APPLE)
    find_package(OpenGL REQUIRED)
    target_link_libraries(Engine PRIVATE OpenGL::GL)
elseif(APPLE)
    find_library(OpenGL_LIBRARY OpenGL)
    target_link_libraries(Engine PRIVATE ${OpenGL_LIBRARY})
endif()