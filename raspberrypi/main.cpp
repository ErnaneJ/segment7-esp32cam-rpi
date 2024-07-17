/**
 * @file main.cpp
 * @brief Download an image from a URL, process it using OpenCV, perform OCR using Tesseract, and display/save results.
 */

#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <curl/curl.h>

/**
 * @brief Downloads an image from a specified URL and saves it to a given path.
 * 
 * @param url The URL of the image to download.
 * @param savePath The local path where the image should be saved.
 * @return true if the download is successful, false otherwise.
 */
bool download_image(const std::string &url, const std::string &savePath)
{
  CURL *curl;
  FILE *fp;
  CURLcode res;
  curl = curl_easy_init();
  if (curl)
  {
    fp = fopen(savePath.c_str(), "wb");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    fclose(fp);
    return true;
  }
  return false;
}

int main(int argc, char **argv)
{
  // Download image from URL and save as ../assets/input.jpg
  std::string url = "http://192.168.3.184/cam.jpg";
  std::string savePath = "../assets/input.jpg";

  if (!download_image(url, savePath))
  {
    std::cerr << "Failed to download image from: " << url << std::endl;
    return 1;
  }

  // Load the downloaded image in color
  cv::Mat image = cv::imread(savePath, cv::IMREAD_COLOR);

  if (image.empty())
  {
    std::cerr << "Error loading image: " << savePath << std::endl;
    return -1;
  }

  // Apply morphology operations
  cv::Mat structuring_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
  cv::morphologyEx(image, image, cv::MORPH_OPEN, structuring_element);
  cv::morphologyEx(image, image, cv::MORPH_CLOSE, structuring_element);

  structuring_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 5));
  for (int i = 0; i < 5; ++i)
  {
    cv::erode(image, image, structuring_element);
  }
  for (int i = 0; i < 5; ++i)
  {
    cv::dilate(image, image, structuring_element);
  }

  structuring_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 1));
  cv::erode(image, image, structuring_element);

  // Display and save the processed image
  cv::imshow("image", image);
  cv::imwrite("../assets/output.png", image);

  // Perform OCR on the image using Tesseract
  tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

  std::string tessdataPath = "../assets/tessdata/";
  if (api->Init(tessdataPath.c_str(), "digits1", tesseract::OEM_DEFAULT))
  {
    std::cerr << "Could not initialize tesseract.\n";
    return 1;
  }

  api->SetImage(image.data, image.cols, image.rows, 3, image.step);

  char *outText = api->GetUTF8Text();
  std::cout << "OCR output: " << outText;

  api->End();
  delete[] outText;
  delete api;

  cv::waitKey(0);

  return 0;
}
