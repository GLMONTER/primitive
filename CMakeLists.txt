cmake_minimum_required(VERSION 3.14)
project(primitive)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ADDITIONAL_CFLAGS} -g")
set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    add_compile_definitions(GLEW_STATIC)
endif()

file(GLOB SRC
        "src/*.cpp"
        "include/imgui/*.cpp"
        "include/imgui/examples/imgui_impl_glfw.cpp"
        "include/imgui/examples/imgui_impl_opengl3.cpp"
        )
if(APPLE)
link_directories(/Users/monter/Documents/primitive/lib/APPLE)
endif()
add_executable(primitive ${SRC})
    if(LINUX)
        FIND_PACKAGE(PkgConfig REQUIRED)
        PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)
        ADD_DEFINITIONS(${GTK3_CFLAGS_OTHER})
    endif()

if(APPLE)
    include_directories(include/APPLE include/APPLE/GL include/APPLE/GLFW include include/glm include/imgui ${GTK3_INCLUDE_DIRS})
endif()
if(LINUX)
    include_directories(include/APPLE include include/glm include/imgui ${GTK3_INCLUDE_DIRS})
endif()

if(WIN32)
    include_directories(include/WIN32 include include/glm include/imgui include/WIN32/GL include/WIN32/nfd)
endif()


if(LINUX)
    LINK_DIRECTORIES(${GTK3_LIBRARY_DIRS})
target_link_libraries(primitive /usr/lib/x86_64-linux-gnu/libGL.so /usr/lib/x86_64-linux-gnu/libglfw.so /usr/lib/x86_64-linux-gnu/libGLEW.so ${GTK3_LIBRARIES})
endif()
if(APPLE)
    target_link_libraries(primitive "-framework OpenGL" "-framework AppKit" libnfd.a libassimp.5.0.1.dylib libglfw.dylib libGLEW.2.2.0.dylib libfmod.dylib)
endif()
if(WIN32)
    find_package(OpenGL REQUIRED)

    target_link_libraries(primitive ${OPENGL_LIBRARIES} ${PROJECT_SOURCE_DIR}/lib/WIN32/assimp-vc142-mt.lib
            ${PROJECT_SOURCE_DIR}/lib/WIN32/fmod_vc.lib ${PROJECT_SOURCE_DIR}/lib/WIN32/glew32s.lib
            ${PROJECT_SOURCE_DIR}/lib/WIN32/glfw3.lib ${PROJECT_SOURCE_DIR}/lib/WIN32/nfd.lib)
endif()

