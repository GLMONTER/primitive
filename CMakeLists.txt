cmake_minimum_required(VERSION 3.16)
project(primitive)

set(CMAKE_CXX_STANDARD 20)

if(WIN32)
    add_compile_definitions(GLEW_STATIC)
endif()

file(GLOB SRC
        "src/*.cpp"
        "include/imgui/*.cpp"
        "include/imgui/examples/imgui_impl_glfw.cpp"
        "include/imgui/examples/imgui_impl_opengl3.cpp"

        )
if(LINUX)
    include_directories(include/LINUX include include/glm include/imgui)
endif()

if(WIN32)
    include_directories(include/WIN32 include include/glm include/imgui include/WIN32/GL)
endif()

    add_executable(primitive ${SRC})
if(LINUX)
target_link_libraries(primitive /usr/lib/x86_64-linux-gnu/libGL.so /usr/lib/x86_64-linux-gnu/libglfw.so /usr/lib/x86_64-linux-gnu/libGLEW.so
        /usr/lib/x86_64-linux-gnu/libassimp.so /home/monter/Documents/primitive/lib/LINUX/libfmod.so)
endif()
if(WIN32)
    find_package(OpenGL REQUIRED)

    target_link_libraries(primitive ${OPENGL_LIBRARIES} C:/Users/logis/Documents/primitive/lib/WIN32/assimp-vc142-mt.lib
            C:/Users/logis/Documents/primitive/lib/WIN32/fmod_vc.lib C:/Users/logis/Documents/primitive/lib/WIN32/glew32s.lib
            C:/Users/logis/Documents/primitive/lib/WIN32/glfw3.lib)
endif()