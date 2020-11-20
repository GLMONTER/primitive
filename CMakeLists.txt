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
add_executable(primitive ${SRC})
    if(LINUX)
        FIND_PACKAGE(PkgConfig REQUIRED)
        PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)
        ADD_DEFINITIONS(${GTK3_CFLAGS_OTHER})
    endif()
if(APPLE)
    find_package(GLEW REQUIRED)
    find_package(ASSIMP REQUIRED)
    link_libraries(${GLEW_LIBRARIES})
    find_package(OpenGL REQUIRED)
endif()
if(APPLE)
    include_directories(include/APPLE include/APPLE/GL include/APPLE/GLFW include include/glm include/imgui ${GTK3_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${ASSIMP_INCLUDE_DIRS} /usr/local/include)
endif()
if(LINUX)
    include_directories(include/APPLE include include/glm include/imgui ${GTK3_INCLUDE_DIRS})
endif()

if(WIN32)
    include_directories(include/WIN32 include include/glm include/imgui include/WIN32/GL)
endif()


if(LINUX)
    LINK_DIRECTORIES(${GTK3_LIBRARY_DIRS})
target_link_libraries(primitive /usr/lib/x86_64-linux-gnu/libGL.so /usr/lib/x86_64-linux-gnu/libglfw.so /usr/lib/x86_64-linux-gnu/libGLEW.so
        /usr/lib/x86_64-linux-gnu/libassimp.so /home/monter/Documents/primitive/lib/LINUX/libfmod.so /home/monter/Documents/primitive/lib/LINUX/libnfd.a  ${GTK3_LIBRARIES})
endif()
if(APPLE)
    LINK_DIRECTORIES(${ASSIMP_LIBRARY_DIRS} /usr/local/lib/ lib/APPLE)
    target_link_libraries(primitive ${OPENGL_LIBRARIES} "-framework AppKit" /Users/monter/Documents/primitive/lib/APPLE/libnfd.a
            ${GLEW_LIBRARIES}
            /Users/monter/Documents/primitive/lib/APPLE/libassimp.5.0.1.dylib /Users/monter/Documents/primitive/lib/APPLE/GLFW/libglfw.3.4.dylib)
endif()
if(WIN32)
    find_package(OpenGL REQUIRED)

    target_link_libraries(primitive ${OPENGL_LIBRARIES} C:/Users/logis/Documents/primitive/lib/WIN32/assimp-vc142-mt.lib
            C:/Users/logis/Documents/primitive/lib/WIN32/fmod_vc.lib C:/Users/logis/Documents/primitive/lib/WIN32/glew32s.lib
            C:/Users/logis/Documents/primitive/lib/WIN32/glfw3.lib)
endif()

