cmake_minimum_required(VERSION 3.16)
project(ANSV2)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(ANSV2 main.cpp
        Implementations/seq_array_n_work.cpp
        Implementations/berkmanOptimal.cpp
        Implementations/shunZhaoOriginal.cpp
        Auxiliary/inputs.cpp
        Auxiliary/printing.cpp
        Auxiliary/printing.h
)
target_include_directories (ANSV2 PUBLIC parlaylib/include)
target_include_directories (ANSV2 PUBLIC parlaylib/examples)

target_link_libraries(ANSV2 PRIVATE pthread)