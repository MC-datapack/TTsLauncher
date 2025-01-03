#include <curl/curl.h>
#include <iostream>
#include <fstream>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::ofstream* file = (std::ofstream*)userp;
    size_t totalSize = size * nmemb;
    file->write((const char*)contents, totalSize);
    return totalSize;
}

bool downloadFile(const std::string& url, const std::string& filename) {
    CURL* curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return false;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);

        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
        curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 1024L); // Optimize buffer size

        // Disable verbose logging
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);

        std::cout << "Download started from: " << url << std::endl;

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            file.close();
            return false;
        }

        curl_easy_cleanup(curl);
        file.close();

        std::cout << "Download completed: " << filename << std::endl;
        return true;
    }
    else {
        std::cerr << "Failed to initialize CURL." << std::endl;
    }
    return false;
}
