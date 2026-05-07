cmake_minimum_required(VERSION 3.20)

if(WIN32)
    set(VCPKG_TARGET_TRIPLET "x64-windows-static" CACHE STRING "Vcpkg triplet" FORCE)
endif()

set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

project(EngineGL2D LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES
    main.cpp
    src/*.cpp
)

add_executable(Engine ${SOURCES})

target_link_libraries(Engine PRIVATE
    glfw
    GLEW::glew_s
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