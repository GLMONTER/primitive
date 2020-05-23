cmake_minimum_required(VERSION 3.16)
project(primitive)

set(CMAKE_CXX_STANDARD 20)



file(GLOB SRC
        "src/*.cpp"
        "include/imgui/*.cpp"
        "include/imgui/examples/imgui_impl_glfw.cpp"
        "include/imgui/examples/imgui_impl_opengl3.cpp"

        )

include_directories(include include/glm include/imgui)
add_executable(primitive ${SRC})

target_link_libraries(primitive /usr/lib/x86_64-linux-gnu/libGL.so /usr/lib/x86_64-linux-gnu/libglfw.so /usr/lib/x86_64-linux-gnu/libGLEW.so
        /usr/lib/x86_64-linux-gnu/libassimp.so /home/monter/Documents/primitive/lib/libfmod.so)