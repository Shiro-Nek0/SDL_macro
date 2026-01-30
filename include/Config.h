#pragma once
#include <string>

namespace Config {
    std::string GetFilePath();
    void Save();
    void Load();
    void OpenFolder();
}