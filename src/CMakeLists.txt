# Создаем СТАТИЧЕСКУЮ БИБЛИОТЕКУ из основных файлов
add_library(
    library_scanner 
    STATIC 
    library_scanner.cpp
)

# Создаем исполняемый файл (саму программу)
add_executable(
    library_scanner_app
    main.cpp
)

# Линкуем исполняемый файл с нашей библиотекой
target_link_libraries(library_scanner_app PRIVATE library_scanner)

# Указываем, где искать заголовочные файлы (для тестов)
# '${CMAKE_CURRENT_SOURCE_DIR}' - это папка 'src'
target_include_directories(library_scanner PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# Указываем стандарт C++17 для обеих целей
target_compile_features(library_scanner PRIVATE cxx_std_17)
target_compile_features(library_scanner_app PRIVATE cxx_std_17)