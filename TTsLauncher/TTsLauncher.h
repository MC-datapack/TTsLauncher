#pragma once

#include "resource.h"
#include "string"

const wchar_t* JSON_URL = L"https://raw.githubusercontent.com/MC-datapack/TTsGames/master/src/main/resources/launcher_info.json";

std::string downloadJarFromJSON(const std::wstring& configURL, HWND hwnd);
void runJar(const std::string& jarPath);