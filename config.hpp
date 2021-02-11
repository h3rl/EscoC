#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>

#include "header.h"
#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

class Config {
public:
    vector<vector<vector<int>>> table;
    bool hidden;
private:
    bool onTop;
    int width, height;
    int x, y;
    json j;
public:
    Config() {
        table = {{},{}};
        onTop = false;
        hidden = false;
    };
    bool updateCFG() {
        // read JSON file
        std::ifstream i("config.json");
        if (i) {
            i >> j;
            try {
                onTop = j["onTop"].get<bool>();
                width = j["window"]["width"].get<int>();
                height = j["window"]["height"].get<int>();
                x = j["window"]["posx"].get<int>();
                y = j["window"]["posy"].get<int>();
                table = {
                    {
                        j["table"][0]["recoil"].get<vector<int>>(),
                        j["table"][0]["delay"].get<vector<int>>()
                    },
                    {
                        j["table"][1]["recoil"].get<vector<int>>(),
                        j["table"][1]["delay"].get<vector<int>>()
                    }
                };
                SetWindowPos(
                    GetConsoleWindow(),
                    (onTop ? HWND_TOPMOST : HWND_NOTOPMOST),
                    x, y, width, height,
                    SWP_SHOWWINDOW | SWP_DRAWFRAME
                );
            }
            catch (...)
            {
                i.close();
                _L("Failed to read config.json, check for typos");
                _L("You can also delete it to make a new one");
                return false;
            }
            i.close();
            return true;
        }
        create();
        return false;
    }
    void create() {
        std::ofstream o("config.json");
        if (o) {
            o << "{" << std::endl;
            o << "  \"onTop\": true," << std::endl;
            o << "  \"window\": {" << std::endl;
            o << "    \"width\": 300," << std::endl;
            o << "    \"height\": 300," << std::endl;
            o << "    \"posx\": -300," << std::endl;
            o << "    \"posy\": 360" << std::endl;
            o << "  }," << std::endl;
            o << "  \"table\": [" << std::endl;
            o << "    {" << std::endl;
            o << "      \"name\": \"Light\"," << std::endl;
            o << "      \"recoil\": [ 7, 6, 5 ]," << std::endl;
            o << "      \"delay\": [ 10, 10, 10 ]" << std::endl;
            o << "    }," << std::endl;
            o << "    {" << std::endl;
            o << "      \"name\": \"Heavy\"," << std::endl;
            o << "      \"recoil\": [ 9, 8, 7 ]," << std::endl;
            o << "      \"delay\": [12, 11, 11] " << std::endl;
            o << "    }" << std::endl;
            o << "  ]" << std::endl;
            o << "}" << std::endl;
            o.close();
            _L("config.json created");
        }
        else {
            _L("failed to create config.json");
        }
    }
    void updateVisibility() {
        hidden = !hidden;
        if (hidden)
            ShowWindow(GetConsoleWindow(), SW_HIDE);
        else
            ShowWindow(GetConsoleWindow(), SW_SHOW);
    }
};