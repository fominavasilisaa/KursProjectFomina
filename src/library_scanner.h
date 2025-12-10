#pragma once
#include <set>
#include <string>
#include <filesystem>

class LibraryScanner {
public:
    static std::set<std::string> scan_project(const std::filesystem::path& root_dir);

private:
    static void extract_libraries(const std::filesystem::path& root_dir,
        std::set<std::string>& out);

    // Новый метод — ОБЯЗАТЕЛЬНО нужен
    static void parse_cmake_block(const std::string& block,
        std::set<std::string>& out);
};
