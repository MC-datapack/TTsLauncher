#include <windows.h>
#include <map>
#include <string>
#include "Config.h"
#include "Utility.h"
#include "TTsLauncher.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static std::map<std::string, std::string> properties;
    static std::wstring propertiesPath;

    switch (uMsg) {
    case WM_CREATE: {
        propertiesPath = getAppDataPath() + L"\\launcher.properties";
        properties = readProperties(propertiesPath);

        HBRUSH hBrush = CreateSolidBrush(RGB(140, 140, 140));
        SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);

        HFONT hFont = CreateFont(
            18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Segoe UI"));

        HWND hButton1 = CreateWindow(L"BUTTON", properties["language"] == "English" ? L"Run JAR" : L"JAR ausführen",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_FLAT, 5, 10, 135, 30, hwnd, (HMENU)1, NULL, NULL);
        SendMessage(hButton1, WM_SETFONT, (WPARAM)hFont, TRUE);

        //HWND hButton2 = CreateWindow(L"BUTTON", properties["language"] == "English" ? L"Toggle Console" : L"Konsole umschalten",
        //    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_FLAT, 10, 100, 135, 30, hwnd, (HMENU)2, NULL, NULL);
        //SendMessage(hButton2, WM_SETFONT, (WPARAM)hFont, TRUE);

        HWND hButton3 = CreateWindow(L"BUTTON", properties["language"] == "English" ? L"Switch Language" : L"Sprache umschalten",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_FLAT, 5, 60, 135, 30, hwnd, (HMENU)3, NULL, NULL);
        SendMessage(hButton3, WM_SETFONT, (WPARAM)hFont, TRUE);

        // Apply console toggle based on properties
        toggleConsole(properties["console"] == "true");
        break;
    }
    case WM_COMMAND: {
        if (LOWORD(wParam) == 1) {
            std::string jarPath = downloadJarFromJSON(JSON_URL);
            if (!jarPath.empty()) {
                runJar(jarPath);
            }
        }
        //else if (LOWORD(wParam) == 2) {
        //    properties["console"] = (properties["console"] == "true") ? "false" : "true";
        //    writeProperties(propertiesPath, properties);
        //    toggleConsole(properties["console"] == "true");
        //}
        else if (LOWORD(wParam) == 3) {
            properties["language"] = (properties["language"] == "English") ? "German" : "English";
            writeProperties(propertiesPath, properties);
            MessageBox(hwnd, properties["language"] == "English" ? L"Language set to English" : L"Sprache auf Deutsch eingestellt", L"Language", MB_OK);
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
