# Создаем исполняемый файл для тестов
add_executable(test_scanner test_scanner.cpp)

# Добавляем путь к заголовочным файлам нашего сканера (src)
# Это нужно, чтобы tests/test_scanner.cpp нашел ../src/library_scanner.h
target_include_directories(test_scanner PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
)

# Привязываем тесты к GTest и к нашей библиотеке 'library_scanner'
target_link_libraries(test_scanner 
    PRIVATE 
    GTest::gtest_main 
    GTest::gtest
    library_scanner   # <-- Линковка только с библиотекой!
)

# Указываем, что нужно использовать C++17
target_compile_features(test_scanner PRIVATE cxx_std_17)

# Включаем тесты в список для запуска (CTest)
enable_testing()
add_test(NAME ScannerTests COMMAND test_scanner)