cmake_minimum_required(VERSION 3.27)
project(halide_course_01_basic_concepts_I)

set(CMAKE_CXX_STANDARD 14)

add_executable(halide_course_01_basic_concepts_I
        00_gamma_correction/src/main.cpp
        01_sepia_effect/src/main.cpp)
