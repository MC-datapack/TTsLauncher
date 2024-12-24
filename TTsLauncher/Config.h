#pragma once
#include <string>
#include <map>

std::map<std::string, std::string> readProperties(const std::wstring& filePath);
void writeProperties(const std::wstring& filePath, const std::map<std::string, std::string>& properties);
std::map<std::string, std::string> getDefaultProperties();
