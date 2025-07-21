cmake_minimum_required(VERSION 4.0)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(mexCSV LANGUAGES CXX)

find_package(Matlab REQUIRED)
#add_library(mexCSV SHARED mexCSV.cpp)

matlab_add_mex(NAME mexCSV 
               SHARED 
               SRC mexCSV.cpp)

target_compile_features(mexCSV PRIVATE cxx_std_17)
