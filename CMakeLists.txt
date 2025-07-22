cmake_minimum_required(VERSION 4.0)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(mexCSV LANGUAGES CXX)

find_package(Matlab REQUIRED)
#add_library(mexCSV SHARED mexCSV.cpp)

# check for from_chars support
set(SRC_FROM_CHARS_TEST "
#include <charconv>
int main() {
    double value;
    char buffer[] = \"123.456\";
    std::from_chars(buffer, buffer+7, value);
    return 0;
}
")
try_compile(HAVE_FROM_CHARS
            SOURCE_FROM_VAR "main.cpp" SRC_FROM_CHARS_TEST
            CXX_STANDARD 17)

matlab_add_mex(NAME mexCSV 
               SHARED 
               SRC mexCSV.cpp)

if(HAVE_FROM_CHARS)
  target_compile_definitions(mexCSV PRIVATE HAVE_FROM_CHARS)
  message("Compiler supports std::from_chars")
  target_compile_features(mexCSV PRIVATE cxx_std_17)
else()
  message("Compiler does not support std::from_chars")
endif()
