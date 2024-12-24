#pragma once
#include <string>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
bool downloadFile(const std::string& url, const std::string& filename);
