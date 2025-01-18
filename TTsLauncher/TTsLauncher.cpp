#include <windows.h>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <map>
#include "Config.h"
#include "Network.h"
#include "Utility.h"
#include "Window.h"
#include <algorithm>
#include <cctype>
//#include "Resource.h"

using json = nlohmann::json;

bool isJavaInstalled() {
    return system("java -version") == 0;
}

#ifdef _WIN32
void runJar(const std::string& jarPath) {
    std::map<std::string, std::string> properties = readProperties(getAppDataPath() + L"\\launcher.properties");
    if (properties["console"] == "true") {
        system(("java -jar " + jarPath).c_str());
    }
    else {
        std::wstring wJarPath(jarPath.begin(), jarPath.end());
        std::wstring command = L"javaw";
        std::wstring arguments = L"-jar " + wJarPath;

        ShellExecute(NULL, L"open", command.c_str(), arguments.c_str(), NULL, SW_HIDE);
    }
}
#else
void runJar(const std::string& jarPath) {
    std::string command;
    if (properties["console"] == "true") {
        command = "java -jar " + jarPath + " > /dev/null 2>&1 &";
    }
    else {
        command = "java -jar " + jarPath;
    }
    system(command.c_str());
}
#endif

// Function to remove trailing spaces
std::string removeTrailingSpaces(const std::string& str) {
    size_t endpos = str.find_last_not_of(" \t");
    return (endpos == std::string::npos) ? "" : str.substr(0, endpos + 1);
}

bool customVersionComparator(const std::string& a, const std::string& b) {
    std::string rc = "rc";
    auto find_rc = [&rc](const std::string& s) {
        return s.find(rc) != std::string::npos;
    };
    bool a_is_rc = find_rc(a);
    bool b_is_rc = find_rc(b);
    if (a_is_rc && b_is_rc) {
        return a > b; // Reverse lexicographical order for "rc" versions 
    } else if (a_is_rc) {
        return true; // "rc" versions come before non "rc" versions 
    } else if (b_is_rc) {
        return false; // Non "rc" versions come after "rc" versions
    } else { 
        return a > b; // Reverse lexicographical order for non "rc" versions 
    }
}

std::string downloadJarFromJSON(const std::wstring& configURL, HWND hwnd, bool latestRelease, bool latestPreRelease, const std::string& version) {
    std::string url = toNarrowString(configURL);

    std::string jsonFile = "launcher_info.json";
    if (!downloadFile(url, jsonFile)) {
        std::cerr << "Failed to download the JSON file from " << url << std::endl;
        return "";
    }

    std::ifstream file(jsonFile);
    if (!file) {
        std::cerr << "Failed to open the JSON file" << std::endl;
        return "";
    }

    nlohmann::json config;
    try {
        file >> config;
    }
    catch (const nlohmann::json::parse_error& e) {
        std::cerr << "Failed to parse the JSON file: " << e.what() << std::endl;
        return "";
    }

    std::string jarUrl;
    if (latestRelease) {
        jarUrl = config["latest"]["release"];
    }
    else if (latestPreRelease) {
        jarUrl = config["latest"]["pre-release"];
    }
    else if (!version.empty()) {
        bool found = false;
        std::vector<std::string> versions;
        for (const auto& item : config["versions"].items()) {
            versions.push_back(item.key());
        }

        std::sort(versions.begin(), versions.end(), customVersionComparator);

        for (const auto& key : versions) {
            if (version.find(key) != std::string::npos) {
                jarUrl = config["versions"][key];
                break;
            }
        }

         if (jarUrl.empty()) {
               std::cerr << "Version not found: [" << version << "]" << std::endl;
               return "";
            }
        }
    else {
        std::cerr << "Invalid version specified or not found in JSON file" << std::endl;
        return "";
    }


    std::string jarPath = "TTsGames.jar";
    if (downloadFile(jarUrl, jarPath)) {
        return jarPath;
    }
    std::cerr << "Failed to download the JAR file from " << jarUrl << std::endl;
    return "";
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    std::map<std::string, std::string> properties;
    std::wstring propertiesPath = getAppDataPath() + L"\\launcher.properties";
    properties = readProperties(propertiesPath);

    const wchar_t CLASS_NAME[] = L"TTs Launcher";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Get screen dimensions
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles
        CLASS_NAME,                     // Window class
        CLASS_NAME,                     // Window text
        WS_OVERLAPPEDWINDOW,            // Window style
        0, 0, screenWidth, screenHeight,
        NULL,       // Parent window
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL) {
        return 1;
    }

    // Make the window fullscreen
    ShowWindow(hwnd, SW_MAXIMIZE);
    SetWindowPos(hwnd, HWND_TOP, 0, 0, screenWidth, screenHeight, SWP_NOZORDER | SWP_FRAMECHANGED);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}