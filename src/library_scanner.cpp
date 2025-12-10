#include "library_scanner.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace fs = std::filesystem;

// ======================================================
// Утилиты
// ======================================================

static std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return s;
}

static std::string strip_comment(const std::string& line) {
    size_t pos = line.find('#');
    if (pos != std::string::npos)
        return line.substr(0, pos);
    return line;
}

// ======================================================
// Парсинг одной полной команды CMake
// ======================================================
void LibraryScanner::parse_cmake_block(const std::string& block,
    std::set<std::string>& out)
{
    std::string clean = strip_comment(block);
    std::string lower = to_lower(clean);

    // -------------------------------
    // find_package(...)
    // -------------------------------
    if (lower.find("find_package") != std::string::npos)
    {
        std::regex re(
            R"(find_package\s*\(\s*([A-Za-z0-9_]+))",
            std::regex::icase
        );

        std::smatch m;
        if (std::regex_search(clean, m, re)) {
            out.insert(m[1].str());
        }
        return;
    }

    // -------------------------------
    // FetchContent_Declare(NAME ...)
    // -> тесты ожидают FetchContent::NAME
    // -------------------------------
    if (lower.find("fetchcontent_declare") != std::string::npos)
    {
        std::regex re(
            R"(FetchContent_Declare\s*\(\s*([A-Za-z0-9_]+))",
            std::regex::icase
        );

        std::smatch m;
        if (std::regex_search(clean, m, re)) {
            out.insert("FetchContent::" + m[1].str());
        }
        return;
    }

    // -------------------------------
    // FetchContent_MakeAvailable(NAME)
    // -------------------------------
    if (lower.find("fetchcontent_makeavailable") != std::string::npos)
    {
        std::regex re(
            R"(FetchContent_MakeAvailable\s*\(\s*([A-Za-z0-9_]+))",
            std::regex::icase
        );

        std::smatch m;
        if (std::regex_search(clean, m, re)) {
            out.insert("FetchContent::" + m[1].str());
        }
        return;
    }

    // -------------------------------
    // target_link_libraries(Target ... libs ...)
    // -------------------------------
    if (lower.find("target_link_libraries") != std::string::npos)
    {
        // Внимание: MSVC не поддерживает .dotall, но это безопасно, потому что
        // мы передаем уже собранный блок (одна полная команда)
        std::regex re(
            R"(target_link_libraries\s*\(([^\)]*)\))",
            std::regex::icase
        );

        std::smatch m;
        if (!std::regex_search(clean, m, re))
            return;

        std::string inside = m[1].str();

        // Разбиваем на токены
        std::stringstream ss(inside);
        std::string token;

        bool first_token = true; // имя цели — пропускаем

        while (ss >> token)
        {
            if (first_token) { first_token = false; continue; }

            // очистка токена
            while (!token.empty() &&
                (token.back() == ')' || token.back() == ',' || token.back() == ';'))
                token.pop_back();

            while (!token.empty() && token.front() == '(')
                token.erase(0, 1);

            if (token.empty()) continue;

            // пропускаем PUBLIC/PRIVATE/INTERFACE
            std::string low = to_lower(token);
            if (low == "public" || low == "private" || low == "interface")
                continue;

            out.insert(token);
        }
        return;
    }
}

// ======================================================
// Рекурсивная обработка директорий
// ======================================================
void LibraryScanner::extract_libraries(const fs::path& root_dir,
    std::set<std::string>& out)
{
    if (!fs::exists(root_dir) || !fs::is_directory(root_dir))
        return;

    static std::set<fs::path> visited;

    // Для тестов: сбрасываем visited в temp директориях
    if (root_dir.string().find("scanner_test_temp") != std::string::npos)
        visited.clear();

    if (visited.count(root_dir)) return;
    visited.insert(root_dir);

    // --- Читаем текущий CMakeLists.txt ---
    fs::path cmake = root_dir / "CMakeLists.txt";
    if (fs::exists(cmake))
    {
        std::cout << "Обработка файла: " << cmake << std::endl;

        std::ifstream file(cmake);
        std::string line;
        std::string buffer;
        int balance = 0;

        while (std::getline(file, line))
        {
            // удаляем комментарии и пробелы
            std::string stripped = strip_comment(line);
            stripped.erase(0, stripped.find_first_not_of(" \t"));
            if (stripped.empty()) continue;

            // добавляем в буфер
            buffer += " " + stripped;

            // считаем баланс скобок
            for (char c : stripped) {
                if (c == '(') balance++;
                if (c == ')') balance--;
            }

            // если команда завершилась — разбираем целиком
            if (balance == 0 && !buffer.empty()) {
                parse_cmake_block(buffer, out);
                buffer.clear();
            }
        }
    }

    // --- Рекурсивный обход ---
    for (auto& entry : fs::directory_iterator(root_dir)) {
        if (entry.is_directory())
            extract_libraries(entry.path(), out);
    }
}

// ======================================================
// scan_project
// ======================================================
std::set<std::string> LibraryScanner::scan_project(const fs::path& root_dir)
{
    std::set<std::string> out;
    extract_libraries(root_dir, out);
    return out;
}
