#include "library_scanner.h"
#include <iostream>

using namespace std;
namespace fs = std::filesystem;

int main() {
    cout << "Утилита для формирования перечня библиотек в CMake проекте" << endl;
    cout << "Введите путь к корневой директории CMake проекта: ";

    string project_path_str;
    getline(cin, project_path_str);

    fs::path project_path(project_path_str);

    if (!fs::exists(project_path) || !fs::is_directory(project_path)) {
        cerr << "Ошибка: Указанный путь не существует или не является директорией." << endl;
        return 1;
    }

    set<string> unique_libraries = LibraryScanner::scan_project(project_path);

    cout << "Полный список уникальных используемых библиотек:" << endl;

    if (unique_libraries.empty()) {
        cout << "Не найдено библиотек, определенных через FindPackage, FetchContent или target_link_libraries." << endl;
    } else {
        int count = 1;
        for (const string& lib : unique_libraries) {
            cout << count++ << ". " << lib << endl;
        }
    }

    cout << "\nНажмите Enter для выхода..." << endl;
    cin.ignore();
    cin.get();

    return 0;
}
