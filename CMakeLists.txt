cmake_minimum_required(VERSION 3.15)
project(primitive)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRC
        "src/*.cpp"
        "include/imgui/*.cpp"
        )



include_directories(include include/glm)
add_executable(primitive ${SRC})

target_link_libraries(primitive /usr/lib/x86_64-linux-gnu/libGL.so /usr/local/lib/libglfw3.a)