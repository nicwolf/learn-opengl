project(learn-opengl)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME}
    ${SRC_LIST}
    shaders/cube.frag
    shaders/cube.vert
    shaders/light.frag
    shaders/light.vert
)
find_package(glfw3 3.2 REQUIRED)
target_link_libraries(learn-opengl glfw)
target_link_libraries(learn-opengl GL)
target_link_libraries(learn-opengl GLEW)
target_link_libraries(learn-opengl SOIL)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
