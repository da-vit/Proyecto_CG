cmake_minimum_required(VERSION 3.16)
project(Proyecto_CG)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(Proyecto_CG ${SOURCES})
target_include_directories(Proyecto_CG PRIVATE src)
target_link_libraries(Proyecto_CG glfw OpenGL::GL)
