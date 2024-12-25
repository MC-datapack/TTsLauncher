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

using json = nlohmann::json;

bool isJavaInstalled() {
    return system("java -version") == 0;
}

void runJar(const std::string& jarPath) {
    system(("java -jar " + jarPath).c_str());
}

std::string downloadJarFromJSON(const std::wstring& configURL) {
    std::string url = toNarrowString(configURL);

    std::string jsonFile = "launcher_info.json";
    if (!downloadFile(url, jsonFile)) {
        std::cerr << "Failed to download the JSON file from " << url << std::endl;
        return "";
    }

    std::ifstream file(jsonFile);
    json config;
    file >> config;

    std::string jarUrl = config["jar-file"];
    std::string jarPath = "TTsGames.jar";

    if (downloadFile(jarUrl, jarPath)) {
        return jarPath;
    }
    return "";
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    std::map<std::string, std::string> properties;
    std::wstring propertiesPath = getAppDataPath() + L"\\launcher.properties";
    properties = readProperties(propertiesPath);

    const wchar_t CLASS_NAME[] = L"TTs Launcher";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        CLASS_NAME,                  // Window text
        WS_OVERLAPPEDWINDOW,            // Window style
        CW_USEDEFAULT, CW_USEDEFAULT, 260, 240,
        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}