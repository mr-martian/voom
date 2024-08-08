cmake_minimum_required(VERSION 3.10)

project(Voom
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(voom main.cc
	compilation_unit.h compilation_unit.cc
	compiler.h compiler.cc
	string.h
)
