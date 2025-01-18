#pragma once

#include "resource.h"
#include "string"

const wchar_t* JSON_URL = L"https://raw.githubusercontent.com/MC-datapack/TTsGames/master/src/main/resources/launcher_info.json";

std::string downloadJarFromJSON(const std::wstring& configURL, HWND hwnd, bool latestRelease, bool latestPreRelease, const std::string& version);
void runJar(const std::string& jarPath);