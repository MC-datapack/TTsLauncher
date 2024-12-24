#pragma once
#include <string>

std::string toNarrowString(const std::wstring& wideString);
void toggleConsole(bool showConsole);
std::wstring getAppDataPath();