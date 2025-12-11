#include "gtest/gtest.h"
#include "../src/library_scanner.h"
#include <fstream>

using namespace std;
namespace fs = std::filesystem;

class ScannerTest : public ::testing::Test {
protected:
    fs::path test_dir = fs::temp_directory_path() / "scanner_test_temp";

    void SetUp() override {
        fs::create_directories(test_dir);
    }

    void TearDown() override {
        fs::remove_all(test_dir);
    }

    void create_cmake_file(const fs::path& path, const string& content) {
        ofstream file(path);
        file << content;
        file.close();
    }
};

TEST_F(ScannerTest, FindsFindPackageLibraries) {
    const fs::path cmake_file = test_dir / "CMakeLists.txt";
    const string content = R"(
        cmake_minimum_required(VERSION 3.10)
        find_package(Qt5 REQUIRED COMPONENTS Core Widgets) # Qt5
        find_package(Boost 1.65 COMPONENTS system filesystem) # Boost
    )";
    create_cmake_file(cmake_file, content);

    set<string> libs = LibraryScanner::scan_project(test_dir);

    EXPECT_EQ(libs.size(), 2);
    EXPECT_TRUE(libs.count("Qt5"));
    EXPECT_TRUE(libs.count("Boost"));
}

TEST_F(ScannerTest, FindsFetchContentProjects) {
    const fs::path cmake_file = test_dir / "CMakeLists.txt";
    const string content = R"(
        include(FetchContent)
        FetchContent_Declare(
            JSON_PROJECT # Имя проекта JSON
            GIT_REPOSITORY https://github.com/nlohmann/json.git
        )
        FetchContent_Declare(fmt URL "http://fmt.com/fmt.zip") # Имя проекта fmt
    )";
    create_cmake_file(cmake_file, content);

    set<string> libs = LibraryScanner::scan_project(test_dir);

    EXPECT_EQ(libs.size(), 2);
    EXPECT_TRUE(libs.count("FetchContent::JSON_PROJECT"));
    EXPECT_TRUE(libs.count("FetchContent::fmt"));
}

TEST_F(ScannerTest, FindsTargetLinkLibrariesAndCleans) {
    const fs::path cmake_file = test_dir / "CMakeLists.txt";
    const string content = R"(
        target_link_libraries(MyExe PRIVATE LibraryA LibraryB) # Либы
        target_link_libraries(AnotherExe PUBLIC LibraryC::component) # Либы с компонентом
    )";
    create_cmake_file(cmake_file, content);

    set<string> libs = LibraryScanner::scan_project(test_dir);

    EXPECT_EQ(libs.size(), 3);
    EXPECT_TRUE(libs.count("LibraryA"));
    EXPECT_TRUE(libs.count("LibraryB"));
    EXPECT_TRUE(libs.count("LibraryC::component"));
}

TEST_F(ScannerTest, HandlesRecursionAndComments) {
    create_cmake_file(test_dir / "CMakeLists.txt", "find_package(RootLib)\nadd_subdirectory(Sub)");
    fs::create_directory(test_dir / "Sub");
    const fs::path sub_cmake_file = test_dir / "Sub" / "CMakeLists.txt";
    const string sub_content = R"(
        # комментарий
        FetchContent_Declare(SubProject)
        target_link_libraries(Target PRIVATE LibFromSub)
    )";
    create_cmake_file(sub_cmake_file, sub_content);

    set<string> libs = LibraryScanner::scan_project(test_dir);

    EXPECT_EQ(libs.size(), 3);
    EXPECT_TRUE(libs.count("RootLib"));
    EXPECT_TRUE(libs.count("FetchContent::SubProject"));
    EXPECT_TRUE(libs.count("LibFromSub"));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
