#include "Config.h"
#include "AppContext.h"
#include "json.hpp"
#include <fstream>
#include <SDL.h>

using json = nlohmann::json;

namespace Config {
    std::string GetFilePath() {
        char *path = SDL_GetPrefPath("MyOrg", "MacroApp");
        if (path) {
            std::string result(path);
            result += "settings.json";
            SDL_free(path);
            return result;
        }
        return "settings.json";
    }

    void Save() {
        json j;
        j["grid"] = {{"cols", GRID_COLS}, {"rows", GRID_ROWS}};
        j["style"] = {{"spacing", buttonSpacing}, {"radius", buttonRadius}};
        std::ofstream file(GetFilePath());
        if (file.is_open()) {
            file << j.dump(4);
            file.close();
        }
    }

    void Load() {
        std::ifstream file(GetFilePath());
        if (!file.is_open()) return;
        try {
            json j;
            file >> j;
            GRID_COLS = j["grid"].value("cols", 15);
            GRID_ROWS = j["grid"].value("rows", 6);
            buttonSpacing = j["style"].value("spacing", 5);
            buttonRadius = j["style"].value("radius", 5);
        } catch (...) {
            SDL_Log("Settings parse error, using defaults.");
        }
    }

    void OpenFolder() {
        char *path = SDL_GetPrefPath("MyOrg", "MacroApp");
        if (path) { SDL_OpenURL(path); SDL_free(path); }
    }
}