/**
 * @file main.cpp
 * @brief Download an image from a URL, process it using OpenCV, perform OCR using Tesseract, and display/save results.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <curl/curl.h>

#define CAM_URL_IMAGE "http://10.7.221.193/cam.jpg"
#define TESS_DATA_PATH "../assets/tessdata"
#define TESS_LANG "digits1"
#define SUFFIX_THRESHOLD_IMAGE_PATH "_threshold.jpg"
#define SUFFIX_FLOOD_FILL_IMAGE_PATH "_flood_fill.jpg"
#define SUFFIX_MORPHOLOGY_IMAGE_PATH "_morphology.jpg"
#define SUFFIX_INPUT_IMAGE_PATH "_ocr.txt"

/**
 * @brief Gets the current date and time formatted as yyyyMMddHHmmss.
 *
 * @return std::string The formatted date and time string.
 */
std::string getCurrentDateTime()
{
  std::time_t now = std::time(nullptr);
  std::tm *ltm = std::localtime(&now);
  std::ostringstream oss;
  oss << (1900 + ltm->tm_year)
      << (1 + ltm->tm_mon)
      << (ltm->tm_mday)
      << (ltm->tm_hour)
      << (ltm->tm_min)
      << (ltm->tm_sec);
  return oss.str();
}

/**
 * @brief Generates a file path with the given prefix and suffix, including the current date and time.
 *
 * @param prefix The prefix of the file path.
 * @param suffix The suffix of the file path.
 * @return std::string The generated file path.
 */
std::string generateFilePath(const std::string &prefix, const std::string &suffix)
{
  return prefix + getCurrentDateTime() + suffix;
}

/**
 * @brief Downloads an image from a specified URL and saves it to a given path.
 *
 * @param url The URL of the image to download.
 * @param savePath The local path where the image should be saved.
 * @return true if the download is successful, false otherwise.
 */
bool downloadImage(const std::string &url, const std::string &savePath)
{
  std::cout << "Downloading image from: " << url << std::endl;
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
    if (res == CURLE_OK)
    {
      std::cout << "Download complete.\n";
      return true;
    }
  }
  std::cout << "Download failed.\n";
  return false;
}

/**
 * @brief Applies edge flood fill to the input image and saves the result.
 *
 * @param inputImage The input image.
 * @param outputPath The path to save the flood-filled image.
 * @return cv::Mat The flood-filled image.
 */
cv::Mat applyEdgeFloodFill(const cv::Mat &inputImage, const std::string &outputPath)
{
  cv::Mat floodFillImage = inputImage.clone();
  cv::floodFill(floodFillImage, cv::Point(0, 0), cv::Scalar(255));
  cv::floodFill(floodFillImage, cv::Point(0, inputImage.rows - 1), cv::Scalar(255));
  cv::floodFill(floodFillImage, cv::Point(inputImage.cols - 1, 0), cv::Scalar(255));
  cv::floodFill(floodFillImage, cv::Point(inputImage.cols - 1, inputImage.rows - 1), cv::Scalar(255));

  cv::imshow(outputPath, floodFillImage);
  cv::imwrite(outputPath, floodFillImage);

  return floodFillImage;
}

/**
 * @brief Applies thresholding to the input image and saves the result.
 *
 * @param inputImage The input image.
 * @param outputPath The path to save the thresholded image.
 * @return cv::Mat The thresholded image.
 */
cv::Mat applyThresholding(const cv::Mat &inputImage, const std::string &outputPath)
{
  std::cout << "Applying thresholding...\n";
  cv::Mat thresholdImage;
  cv::cvtColor(inputImage, thresholdImage, cv::COLOR_BGR2GRAY);
  cv::threshold(thresholdImage, thresholdImage, 110, 255, cv::THRESH_BINARY);
  cv::imshow(outputPath, thresholdImage);
  cv::imwrite(outputPath, thresholdImage);
  return thresholdImage;
}

/**
 * @brief Corrects the orientation of the input image using Hough Line Transform and rotation.
 *
 * @param inputImage The input image.
 * @return cv::Mat The image with corrected orientation.
 */
cv::Mat correctImageOrientation(const cv::Mat &inputImage)
{
  std::cout << "Correcting image orientation...\n";
  cv::Mat edges;
  cv::Canny(inputImage, edges, 50, 200, 3);

  std::vector<cv::Vec2f> lines;
  cv::HoughLines(edges, lines, 1, CV_PI / 180, 100);

  double angle = 0.0;
  int count = 0;
  for (size_t i = 0; i < lines.size(); i++)
  {
    float rho = lines[i][0], theta = lines[i][1];
    if (theta > CV_PI / 4 && theta < 3 * CV_PI / 4) // Consider only nearly horizontal lines
    {
      angle += theta;
      count++;
    }
  }

  if (count > 0)
  {
    angle = angle / count - CV_PI / 2;
  }

  cv::Mat rotatedImage;
  cv::Point2f center(inputImage.cols / 2.0F, inputImage.rows / 2.0F);
  cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, angle * 180 / CV_PI, 1.0);
  cv::warpAffine(inputImage, rotatedImage, rotationMatrix, inputImage.size());

  cv::imshow("Corrected Orientation Image", rotatedImage);

  rotatedImage = applyEdgeFloodFill(rotatedImage, generateFilePath("../assets/", SUFFIX_FLOOD_FILL_IMAGE_PATH));

  // remove undesired horizontal lines
  cv::Mat structuringElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
  cv::dilate(rotatedImage, rotatedImage, structuringElement);
  cv::dilate(rotatedImage, rotatedImage, structuringElement);
  cv::erode(rotatedImage, rotatedImage, structuringElement);
  cv::erode(rotatedImage, rotatedImage, structuringElement);

  cv::imshow("Corrected and Clean Orientation Image", rotatedImage);
  return rotatedImage;
}

/**
 * @brief Applies morphological operations to the thresholded image and saves the result.
 *
 * @param inputImage The thresholded image.
 * @param outputPath The path to save the morphologically processed image.
 * @return cv::Mat The morphologically processed image.
 */
cv::Mat applyMorphology(const cv::Mat &inputImage, const std::string &outputPath)
{
  std::cout << "Applying morphological operations...\n";
  cv::Mat image = inputImage.clone();
  cv::Mat structuringVerticalElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 5));
  cv::Mat structuringHorizontalElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 1));
  cv::Mat structuringElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));

  cv::dilate(image, image, structuringVerticalElement);
  cv::dilate(image, image, structuringVerticalElement);
  cv::dilate(image, image, structuringVerticalElement);
  cv::dilate(image, image, structuringVerticalElement);
  cv::erode(image, image, structuringVerticalElement);
  cv::erode(image, image, structuringVerticalElement);
  cv::erode(image, image, structuringVerticalElement);
  cv::erode(image, image, structuringVerticalElement);
  cv::erode(image, image, structuringVerticalElement);
  cv::erode(image, image, structuringVerticalElement);
  cv::erode(image, image, structuringVerticalElement);
  cv::erode(image, image, structuringVerticalElement);
  cv::erode(image, image, structuringVerticalElement);
  cv::erode(image, image, structuringVerticalElement);
  cv::erode(image, image, structuringVerticalElement);
  cv::erode(image, image, structuringVerticalElement);
  cv::erode(image, image, structuringVerticalElement);
  cv::erode(image, image, structuringVerticalElement);
  cv::erode(image, image, structuringVerticalElement);
  cv::dilate(image, image, structuringVerticalElement);
  cv::dilate(image, image, structuringHorizontalElement);
  cv::erode(image, image, structuringHorizontalElement);
  cv::erode(image, image, structuringHorizontalElement);
  cv::erode(image, image, structuringHorizontalElement);
  cv::erode(image, image, structuringHorizontalElement);
  cv::erode(image, image, structuringHorizontalElement);
  cv::erode(image, image, structuringHorizontalElement);
  cv::erode(image, image, structuringHorizontalElement);
  cv::dilate(image, image, structuringHorizontalElement);
  cv::dilate(image, image, structuringHorizontalElement);
  cv::dilate(image, image, structuringHorizontalElement);
  cv::dilate(image, image, structuringHorizontalElement);

  for(int row = 0; row < image.rows; row++)
  {
    for(int col = 0; col < image.cols; col++)
    {
      if(image.at<uchar>(row, col) == 255)
      {
        image.at<uchar>(row, col) = 0;
      }
      else
      {
        image.at<uchar>(row, col) = 255;
      }
    }
  }

  cv::imshow(outputPath, image);
  cv::imwrite(outputPath, image);
  std::cout << "Morphological processing complete.\n";
  return image;
}


/**
 * @brief Performs OCR on the input image using Tesseract.
 *
 * @param image The image on which OCR is to be performed.
 */
float performOCR(const std::string &outputPath)
{
  float value = 0.0;
  cv::Mat image = cv::imread(outputPath, cv::IMREAD_COLOR);
  std::cout << "Performing OCR...\n";
  tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
  if (api->Init(TESS_DATA_PATH, TESS_LANG, tesseract::OEM_DEFAULT))
  {
    std::cerr << "Could not initialize tesseract.\n";
    return value;
  }
  std::cout << "Tesseract initialized.\n";
  api->SetImage(image.data, image.cols, image.rows, 3, image.step);
  std::cout << "Image set for OCR.\n";
  char *outText = api->GetUTF8Text();
  if (outText != nullptr && outText[0] != '\0')
  {
    std::cout << "OCR output: " << outText << std::endl;
    value = atof(outText) / 1000;
  }
  else
  {
    std::cout << "OCR output is empty.\n";
  }
  api->End();
  delete[] outText;
  delete api;
  std::cout << "OCR complete.\n";

  return value;
}

int main(int argc, char **argv)
{
  std::cout << "Starting image processing...\n";

  std::string inputPath = "../assets/input4.jpg"; // generateFilePath("../assets/", SUFFIX_INPUT_IMAGE_PATH);
  std::string thresholdPath = generateFilePath("../assets/", SUFFIX_THRESHOLD_IMAGE_PATH);
  std::string morphologyPath = generateFilePath("../assets/", SUFFIX_MORPHOLOGY_IMAGE_PATH);

  // if (!downloadImage(CAM_URL_IMAGE, inputPath))
  // {
  //     std::cerr << "Failed to download image from: " << CAM_URL_IMAGE << std::endl;
  //     return 1;
  // }
  std::cout << "Image saved to: " << inputPath << std::endl;

  cv::Mat image = cv::imread(inputPath, cv::IMREAD_COLOR);
  if (image.empty())
  {
    std::cerr << "Error loading image: " << inputPath << std::endl;
    return -1;
  }
  std::cout << "Image loaded successfully.\n";
  cv::imshow("Captured Image", image);

  cv::Mat thresholdImage = applyThresholding(image, thresholdPath);
  cv::Mat correctedImageHorizontal = correctImageOrientation(thresholdImage);
  cv::Mat processedImage = applyMorphology(correctedImageHorizontal, morphologyPath);

  float value = performOCR(morphologyPath);

  std::cout << "Value: " << value << std::endl;

  cv::waitKey(0);
  std::cout << "Exiting...\n";
  return 0;
}