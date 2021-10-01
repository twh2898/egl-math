cmake_minimum_required(VERSION 3.15...3.20)
project(project VERSION 0.0.0 LANGUAGES CXX)

find_package(fmt REQUIRED CONFIG)
find_package(OpenGL REQUIRED)

set(TARGET app)
add_executable(${TARGET}
    main.cpp
    table.hpp
    context.hpp
    Shader.cpp
    Shader.hpp
    vbo.hpp
    vbo.cpp)
target_compile_features(${TARGET} PRIVATE cxx_std_17)

target_link_libraries(${TARGET} PRIVATE fmt::fmt OpenGL::EGL)

