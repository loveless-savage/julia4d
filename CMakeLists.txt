cmake_minimum_required(VERSION 3.21)
project(Code)

set(CMAKE_CXX_STANDARD 14)
find_package(OpenGL REQUIRED)
message(STATUS "OpenGL found at ${OPENGL_LIBRARIES}")

link_directories(libs libs/GLFW libs/GL)

include_directories(inc libs)
include_directories(../glm)

add_executable(julia4d
        src/camera.cpp
        src/main.cpp
        src/mat4.cpp
        src/rayMarcher.cpp
        src/vec4.cpp
        src/glwindow.cpp
        src/multipasswindow.cpp
        src/gldatashells.cpp
    )

target_link_libraries(julia4d glfw GLEW ${OPENGL_LIBRARIES})

