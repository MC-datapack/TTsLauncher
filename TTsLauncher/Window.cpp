#include <windows.h>
#include <map>
#include <string>
#include "Config.h"
#include "Utility.h"
#include "TTsLauncher.h"
#include "Resource.h"

#pragma comment(lib, "Msimg32.lib")

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static std::map<std::string, std::string> properties;
    static std::wstring propertiesPath;
    static HBITMAP hBitmap = NULL;
    static HWND hButton1, hButton2, hButton3;

    switch (uMsg) {
    case WM_CREATE: {
        propertiesPath = getAppDataPath() + L"\\launcher.properties";
        properties = readProperties(propertiesPath);

        // Load the background image from resources
        hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_TTSGAMES));

        if (!hBitmap) {
            MessageBox(hwnd, L"Failed to load bitmap", L"Error", MB_OK);
        }

        HFONT hFont = CreateFont(
            18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Segoe UI"));

        hButton1 = CreateWindow(L"BUTTON", properties["language"] == "English" ? L"Run newest TTsGames version" : L"Neuste TTsGames Version starten",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 5, 10, 230, 30, hwnd, (HMENU)1, NULL, NULL);
        SendMessage(hButton1, WM_SETFONT, (WPARAM)hFont, TRUE);

        hButton2 = CreateWindow(L"BUTTON", properties["language"] == "English" ? L"Toggle Logs" : L"Logs umschalten",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 5, 125, 230, 30, hwnd, (HMENU)2, NULL, NULL);
        SendMessage(hButton2, WM_SETFONT, (WPARAM)hFont, TRUE);

        hButton3 = CreateWindow(L"BUTTON", properties["language"] == "English" ? L"Switch Language" : L"Sprache umschalten",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 5, 85, 230, 30, hwnd, (HMENU)3, NULL, NULL);
        SendMessage(hButton3, WM_SETFONT, (WPARAM)hFont, TRUE);

        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Draw the background image
        if (hBitmap) {
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

            BITMAP bitmap;
            GetObject(hBitmap, sizeof(bitmap), &bitmap);
            BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

            SelectObject(hdcMem, hbmOld);
            DeleteDC(hdcMem);
        }
        else {
            MessageBox(hwnd, L"Bitmap is null", L"Error", MB_OK);
        }

        EndPaint(hwnd, &ps);
        break;
    }
    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
        HDC hdc = pDIS->hDC;
        RECT rc = pDIS->rcItem;

        HBRUSH hBrush = CreateSolidBrush(RGB(50, 50, 50)); // Dark background color
        FillRect(hdc, &rc, hBrush);

        // Draw 3D effect
        HPEN hPenLight = CreatePen(PS_SOLID, 2, RGB(150, 150, 150)); // Light shadow
        HPEN hPenDark = CreatePen(PS_SOLID, 2, RGB(100, 100, 100)); // Dark shadow

        SelectObject(hdc, hPenLight);
        MoveToEx(hdc, rc.left, rc.bottom - 1, NULL);
        LineTo(hdc, rc.left, rc.top);
        LineTo(hdc, rc.right - 1, rc.top);

        SelectObject(hdc, hPenDark);
        MoveToEx(hdc, rc.left, rc.bottom - 1, NULL);
        LineTo(hdc, rc.right - 1, rc.bottom - 1);
        LineTo(hdc, rc.right - 1, rc.top);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255)); // Set text color to black

        // Draw button text
        DrawText(hdc, pDIS->hwndItem == hButton1 ? (properties["language"] == "English" ? L"Run newest TTsGames version" : L"Neuste TTsGames Version starten") :
            pDIS->hwndItem == hButton2 ? (properties["language"] == "English" ? L"Toggle Logs" : L"Logs umschalten") :
            (properties["language"] == "English" ? L"Switch Language" : L"Sprache umschalten"), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        if (pDIS->itemState & ODS_FOCUS) {
            DrawFocusRect(hdc, &rc);
        }

        DeleteObject(hBrush);
        DeleteObject(hPenLight);
        DeleteObject(hPenDark);

        break;
    }
    case WM_COMMAND: {
        if (LOWORD(wParam) == 1) {
            MessageBox(hwnd, properties["language"] == "English" ? L"Downloading Jar file" : L"Jar Datei wird heruntergeladen", L"Jar", MB_OK);
            std::string jarPath = downloadJarFromJSON(JSON_URL, hwnd);
            if (!jarPath.empty()) {
                runJar(jarPath);
            }
        }
        else if (LOWORD(wParam) == 2) {
            properties["console"] = properties["console"] == "false" ? "true" : "false";
            writeProperties(propertiesPath, properties);
            MessageBox(hwnd, (properties["language"] == "English" ? (L"Display logs was set to: " + std::wstring(properties["console"].begin(), properties["console"].end())) : (L"Logs anzeigen wurde auf: \"" + std::wstring(properties["console"].begin(), properties["console"].end()) + L"\" gesetzt")).c_str(), L"Information", MB_OK);
        }
        else if (LOWORD(wParam) == 3) {
            properties["language"] = (properties["language"] == "English") ? "Deutsch" : "English";
            writeProperties(propertiesPath, properties);
            MessageBox(hwnd, properties["language"] == "English" ? L"Language set to English \nRestart to change the language." : L"Sprache auf Deutsch eingestellt \nNeustarten um die Sprache zu ändern.", L"Language", MB_OK);
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

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"MainWindowClass";

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, wc.lpszClassName, L"Application", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
