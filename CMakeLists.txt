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
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
        link_directories(${PROJECT_SOURCE_DIR}/lib/APPLE/arm64)
    else ()
        link_directories(${PROJECT_SOURCE_DIR}/lib/APPLE/x86_64)
    endif ()
endif()
add_executable(primitive ${SRC})
    if(UNIX AND NOT APPLE)
        find_package(GLEW REQUIRED)
        find_package(OpenGL REQUIRED)
        find_package(glfw3 REQUIRED)
        find_package(assimp REQUIRED)
        FIND_PACKAGE(PkgConfig REQUIRED)

        PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)
        ADD_DEFINITIONS(${GTK3_CFLAGS_OTHER})
    endif()

if(APPLE)
    include_directories(include/APPLE include/APPLE/fmod include/APPLE/GL include/APPLE/GLFW include include/glm include/imgui ${GTK3_INCLUDE_DIRS})
endif()
if(UNIX AND NOT APPLE)
    include_directories(include/LINUX include include/glm include/imgui ${GTK3_INCLUDE_DIRS} ${assimp_INCLUDE_DIRS})
endif()

if(WIN32)
    include_directories(include/WIN32 include include/glm include/imgui include/WIN32/GL include/WIN32/nfd)
endif()


if(UNIX AND NOT APPLE)
    LINK_DIRECTORIES(${GTK3_LIBRARY_DIRS})
target_link_libraries(primitive ${PROJECT_SOURCE_DIR}/lib/LINUX/libnfd.a ${PROJECT_SOURCE_DIR}/lib/LINUX/libfmod.so assimp glfw ${OPENGL_LIBRARIES}
   ${GTK3_LIBRARIES} ${GLEW_LIBRARIES})
endif()
if(APPLE)
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
        target_link_libraries(primitive "-framework OpenGL -framework AppKit -framework IOKit" libnfd.a libassimp.5.dylib libglfw3_arm64.a libGLEW.dylib libfmod.dylib)
    else ()
        target_link_libraries(primitive "-framework OpenGL -framework AppKit -framework IOKit" libnfd_x86_64.a libassimp.5_x64_86.dylib libglfw3_x86_64.a libGLEW_x86_64.dylib libfmod.dylib)
    endif ()
endif()
if(WIN32)
    find_package(OpenGL REQUIRED)

    target_link_libraries(primitive ${OPENGL_LIBRARIES} ${PROJECT_SOURCE_DIR}/lib/WIN32/assimp-vc142-mt.lib
            ${PROJECT_SOURCE_DIR}/lib/WIN32/fmod_vc.lib ${PROJECT_SOURCE_DIR}/lib/WIN32/glew32s.lib
            ${PROJECT_SOURCE_DIR}/lib/WIN32/glfw3.lib ${PROJECT_SOURCE_DIR}/lib/WIN32/nfd.lib)
endif()

