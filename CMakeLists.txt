cmake_minimum_required(VERSION 4.0)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(mexCSV LANGUAGES CXX)

find_package(Matlab REQUIRED)

option(USE_FAST_FLOAT "Use the fast_float library for from_chars support" OFF)

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

# desire static linking for easier mex distribution
target_link_options(mexCSV PRIVATE -static)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/extern/fast_float" AND USE_FAST_FLOAT)
  target_include_directories(mexCSV PRIVATE extern/fast_float/include)
  message(STATUS "Using fast_float library for from_chars support")
elseif(HAVE_FROM_CHARS)
  target_compile_definitions(mexCSV PRIVATE HAVE_FROM_CHARS)
  target_compile_features(mexCSV PRIVATE cxx_std_17)
  message(STATUS "Compiler supports std::from_chars")
else()
  message(STATUS "Compiler does not support std::from_chars")
endif()
