add_executable(test_scanner test_scanner.cpp)

target_include_directories(test_scanner PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(test_scanner 
    PRIVATE 
    GTest::gtest_main 
    GTest::gtest
    library_scanner   # <-- Линковка только с библиотекой!
)

target_compile_features(test_scanner PRIVATE cxx_std_17)

enable_testing()
add_test(NAME ScannerTests COMMAND test_scanner)
