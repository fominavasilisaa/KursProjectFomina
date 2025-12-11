add_library(
    library_scanner 
    STATIC 
    library_scanner.cpp
)

add_executable(
    library_scanner_app
    main.cpp
)

target_link_libraries(library_scanner_app PRIVATE library_scanner)

target_include_directories(library_scanner PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
target_compile_features(library_scanner PRIVATE cxx_std_17)
target_compile_features(library_scanner_app PRIVATE cxx_std_17)
