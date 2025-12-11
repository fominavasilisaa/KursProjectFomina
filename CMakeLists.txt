cmake_minimum_required(VERSION 3.14)

project(LibraryScanner CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "Force gtest to use shared CRT")

FetchContent_MakeAvailable(googletest)

add_subdirectory(src)
add_subdirectory(tests)
