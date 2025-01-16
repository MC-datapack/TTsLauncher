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

std::string downloadJarFromJSON(const std::wstring& configURL, HWND hwnd) {
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

    json config;
    try {
        file >> config;
    }
    catch (const json::parse_error& e) {
        std::cerr << "Failed to parse the JSON file: " << e.what() << std::endl;
        return "";
    }

    //javaUrl = toNarrowString(config["jdk-download"]);

    std::string jarUrl = config["jar-file"];
    std::string jarPath = "TTsGames.jar";

    if (downloadFile(jarUrl, jarPath)) {
        return jarPath;
    }
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