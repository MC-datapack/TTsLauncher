#include <windows.h>
#include <shlobj.h>
#include <string>

// Function to convert wide string to narrow string
std::string toNarrowString(const std::wstring& wideString) {
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wideString.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string narrowString(bufferSize, 0);
    WideCharToMultiByte(CP_UTF8, 0, wideString.c_str(), -1, &narrowString[0], bufferSize, nullptr, nullptr);
    return narrowString;
}

std::wstring stringToWstring(const std::string& str) { 
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0); 
    std::wstring wstr(size_needed, 0); 
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed); return wstr; 
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
