#include <fstream>
#include <string>
#include <map>
#include <iostream>
#include "Utility.h"

// Default properties
std::map<std::string, std::string> getDefaultProperties() {
    std::map<std::string, std::string> defaultProperties;
    defaultProperties["language"] = "English";
    defaultProperties["console"] = "false";
    return defaultProperties;
}

// Function to write properties to the configuration file
void writeProperties(const std::wstring& filePath, const std::map<std::string, std::string>& properties) {
    std::ofstream file(filePath);
    if (file.is_open()) {
        for (const auto& pair : properties) {
            file << pair.first << "=" << pair.second << "\n";
        }
        file.close();
    }
    else {
        std::cerr << "Failed to open file for writing: " << toNarrowString(filePath) << std::endl;
    }
}

// Function to read properties from the configuration file
std::map<std::string, std::string> readProperties(const std::wstring& filePath) {
    std::ifstream file(filePath);
    std::map<std::string, std::string> properties;

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            size_t delimiterPos = line.find('=');
            if (delimiterPos != std::string::npos) {
                std::string key = line.substr(0, delimiterPos);
                std::string value = line.substr(delimiterPos + 1);
                properties[key] = value;
            }
        }
        file.close();
    }
    else {
        properties = getDefaultProperties();
        writeProperties(filePath, properties);
    }

    std::map<std::string, std::string> defaultProperties = getDefaultProperties();
    for (const auto& pair : defaultProperties) {
        if (properties.find(pair.first) == properties.end()) {
            properties[pair.first] = pair.second;
        }
    }

    return properties;
}
