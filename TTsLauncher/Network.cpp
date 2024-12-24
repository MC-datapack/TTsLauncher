#include <curl/curl.h>
#include <fstream>
#include <iostream>
#include <string>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

bool downloadFile(const std::string& url, const std::string& filename) {
    CURL* curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
        std::string readBuffer;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);

        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        std::cout << "Download started from: " << url << std::endl;

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            return false;
        }

        curl_easy_cleanup(curl);

        std::ofstream file(filename, std::ios::binary);
        file.write(readBuffer.c_str(), readBuffer.size());
        file.close();

        std::cout << "Download completed: " << filename << std::endl;
        return true;
    }
    else {
        std::cerr << "Failed to initialize CURL." << std::endl;
    }
    return false;
}
