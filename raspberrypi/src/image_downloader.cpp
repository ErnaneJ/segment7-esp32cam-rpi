#include "image_downloader.h"
#include <curl/curl.h>
#include <fstream>

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    std::ofstream *outputFile = static_cast<std::ofstream *>(userp);
    size_t totalSize = size * nmemb;
    outputFile->write(static_cast<char *>(contents), totalSize);
    return totalSize;
}

bool downloadImage(const std::string &url, const std::string &outputPath)
{
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        return false;
    }

    std::ofstream outputFile(outputPath, std::ios::binary);
    if (!outputFile.is_open())
    {
        curl_easy_cleanup(curl);
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outputFile);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    outputFile.close();

    return res == CURLE_OK;
}
