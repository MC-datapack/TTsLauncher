#include <windows.h>
#include <CommCtrl.h>
#include <map>
#include <string>
#include "Config.h"
#include "Utility.h"
#include "TTsLauncher.h"
#include "Resource.h"

#pragma comment(lib, "Msimg32.lib")
#pragma comment(lib, "comctl32.lib")

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static std::map<std::string, std::string> properties;
    static std::wstring propertiesPath;
    static HBITMAP hBitmap = NULL;
    static HWND hButton1, hButton2, hButton3, hButton4, hProgressBar;

    switch (uMsg) {
    case WM_CREATE: {
        propertiesPath = getAppDataPath() + L"\\launcher.properties";
        properties = readProperties(propertiesPath);

        // Load the background image from resources
        hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(properties["darkMode"] == "true" ? IDB_TTSGAMESDARK : IDB_TTSGAMES));

        if (!hBitmap) {
            MessageBox(hwnd, L"Failed to load bitmap", L"Error", MB_OK);
        }

        HFONT hFont = CreateFont(
            18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Segoe UI"));

        // Get the dimensions of the window
        RECT rect;
        GetClientRect(hwnd, &rect);
        int windowWidth = rect.right - rect.left;
        int windowHeight = rect.bottom - rect.top;

        // Calculate the center positions
        int centerX = windowWidth / 2;
        int centerY = windowHeight / 2 - (windowHeight / 11);

        // Button dimensions
        int buttonWidth = 250;
        int buttonHeight = 50;
        int buttonSpacing = 10;

        // Calculate button positions
        int button1X = centerX - (buttonWidth / 2); 
        int button1Y = centerY - buttonHeight - 1.5 * buttonSpacing; 
        int button2X = centerX - (buttonWidth / 2);
        int button2Y = centerY;
        int button3X = centerX - (buttonWidth / 2); 
        int button3Y = centerY + buttonHeight + 1.5 * buttonSpacing; 
        int button4X = centerX - (buttonWidth / 2);
        int button4Y = centerY + 2 * (buttonHeight + buttonSpacing);

        // Create buttons at the new positions
        hButton1 = CreateWindow(L"BUTTON", properties["language"] == "English" ? L"Run newest TTsGames version" : L"Neuste TTsGames Version starten",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, button1X, button1Y, buttonWidth, buttonHeight, hwnd, (HMENU)1, NULL, NULL);
        SendMessage(hButton1, WM_SETFONT, (WPARAM)hFont, TRUE);

        hButton2 = CreateWindow(L"BUTTON", properties["language"] == "English" ? L"Toggle Logs" : L"Logs umschalten",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, button2X, button2Y, buttonWidth, buttonHeight, hwnd, (HMENU)2, NULL, NULL);
        SendMessage(hButton2, WM_SETFONT, (WPARAM)hFont, TRUE);

        hButton3 = CreateWindow(L"BUTTON", properties["language"] == "English" ? L"Switch Language" : L"Sprache umschalten",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, button3X, button3Y, buttonWidth, buttonHeight, hwnd, (HMENU)3, NULL, NULL);
        SendMessage(hButton3, WM_SETFONT, (WPARAM)hFont, TRUE);

        hButton4 = CreateWindow(L"BUTTON", properties["language"] == "English" ? L"Dark/Light Mode" : L"Dunkel/Hell",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, button4X, button4Y, buttonWidth, buttonHeight, hwnd, (HMENU)4, NULL, NULL);
        SendMessage(hButton4, WM_SETFONT, (WPARAM)hFont, TRUE);

        // Create the progress bar
        hProgressBar = CreateWindowEx(0, PROGRESS_CLASS, NULL, WS_CHILD | WS_VISIBLE,
            10, windowHeight - 50, windowWidth - 20, 30, hwnd, (HMENU)4, NULL, NULL);
        SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
        SendMessage(hProgressBar, PBM_SETPOS, 0, 0);
        SendMessage(hProgressBar, PBM_SETBARCOLOR, 0, (LPARAM)RGB(0, 255, 0));
        SendMessage(hProgressBar, PBM_SETSTEP, 0, 1);

        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Fill the entire background with white
        RECT rect;
        GetClientRect(hwnd, &rect);
        HBRUSH hBrushWhite = CreateSolidBrush(properties["darkMode"] == "true" ? RGB(0, 0, 0) : RGB(255, 255, 255));
        FillRect(hdc, &rect, hBrushWhite);
        DeleteObject(hBrushWhite);

        // Draw the 1024x1024 background image centered in the window
        if (hBitmap) {
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

            BITMAP bitmap;
            GetObject(hBitmap, sizeof(bitmap), &bitmap);

            // Calculate the position to center the image
            int imageX = (rect.right - rect.left - bitmap.bmWidth) / 2;
            int imageY = (rect.bottom - rect.top - bitmap.bmHeight) / 2;

            BitBlt(hdc, imageX, imageY, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

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

        HBRUSH hBrush = CreateSolidBrush(properties["darkMode"] == "true" ? RGB(0, 0, 0) : RGB(50, 50, 50)); // Dark background color
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
            pDIS->hwndItem == hButton3 ? (properties["language"] == "English" ? L"Switch Language" : L"Sprache umschalten") :
            (properties["language"] == "English" ? L"Dark/Light Mode" : L"Dunkel/Hell"), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
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

            // Simulate download progress for demonstration
            for (int i = 0; i <= 100; i++) {
                SendMessage(hProgressBar, PBM_SETPOS, i, 0);
                Sleep(50); // Simulate time delay
            }

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
        else if (LOWORD(wParam) == 4) {
            properties["darkMode"] = (properties["darkMode"] == "true") ? "false" : "true";
            writeProperties(propertiesPath, properties);
            MessageBox(hwnd, properties["language"] == "English" ? L"Dark Mode was changed \nRestart to see it." : L"Dunkler Modus wirde geändert \nNeustarten um es zu sehen.", L"Dark Mode", MB_OK);
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