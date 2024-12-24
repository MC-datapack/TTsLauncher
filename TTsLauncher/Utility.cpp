#include <string>
#include <windows.h>
#include <shlobj.h>

// Convert wide string to narrow string
std::string toNarrowString(const std::wstring& wideString) {
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wideString.c_str(), -1, NULL, 0, NULL, NULL);
    if (bufferSize == 0) {
        return "";
    }
    std::string narrowString(bufferSize, 0);
    WideCharToMultiByte(CP_UTF8, 0, wideString.c_str(), -1, &narrowString[0], bufferSize, NULL, NULL);
    return narrowString;
}

std::wstring getAppDataPath() {
    wchar_t* appData;
    HRESULT result = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &appData);
    if (result == S_OK) {
        std::wstring path(appData);
        CoTaskMemFree(appData);
        return path + L"\\TTsGames";
    }
    return L"";
}

void toggleConsole(bool showConsole) {
    if (showConsole) {
        AllocConsole();
        FILE* console;
        freopen_s(&console, "CONOUT$", "w", stdout);
        freopen_s(&console, "CONOUT$", "w", stderr);
    }
    else {
        FreeConsole();
    }
}
