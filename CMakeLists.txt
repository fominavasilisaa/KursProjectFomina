# Указываем минимальную версию CMake
cmake_minimum_required(VERSION 3.14)

# Задаем имя проекта и требуемый язык C++17
project(LibraryScanner CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------------------------------
# 1. Загрузка Google Test (FetchContent)
# ----------------------------------------------------
include(FetchContent)

# Объявляем, что нам нужен GTest, и где его взять
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)

# ПРИНУДИТЕЛЬНЫЙ ФИКС RuntimeLibrary ТОЛЬКО ДЛЯ GTEST
# Говорим GTest использовать MDd, чтобы устранить конфликт с твоим кодом.
set(gtest_force_shared_crt ON CACHE BOOL "Force gtest to use shared CRT")

# Загружаем содержимое
FetchContent_MakeAvailable(googletest)

# ----------------------------------------------------
# 2. Сборка модулей
# ----------------------------------------------------
add_subdirectory(src)
add_subdirectory(tests)