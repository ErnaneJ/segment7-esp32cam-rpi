#include <iostream>
#include <opencv2/opencv.hpp>
#include "image_processor.h"
#include "image_downloader.h"
#include "ocr_processor.h"
#include "notifier.h"

#define CAM_URL_IMAGE "http://10.7.221.193/cam.jpg"

bool debug = false;

int main(int argc, char **argv)
{
  for (int i = 1; i < argc; ++i)
  {
    if (std::string(argv[i]) == "--debug")
    {
      debug = true;
      break;
    }
  }

  while (true)
  {
    if (debug)
      std::cout << "Starting image processing...\n";

    std::string inputPath = generateFilePath("../assets/", SUFFIX_INPUT_IMAGE_PATH);
    std::string perspectivePath = generateFilePath("../assets/", SUFFIX_PERSPECTIVE_IMAGE_PATH, debug);
    std::string thresholdPath = generateFilePath("../assets/", SUFFIX_THRESHOLD_IMAGE_PATH, debug);
    std::string morphologyPath = generateFilePath("../assets/", SUFFIX_MORPHOLOGY_IMAGE_PATH, debug);
    std::string floodFillPath = generateFilePath("../assets/", SUFFIX_FLOOD_FILL_IMAGE_PATH, debug);

    if (!downloadImage(CAM_URL_IMAGE, inputPath))
    {
        std::cerr << "Failed to download image from: " << CAM_URL_IMAGE << std::endl;
        return 1;
    }
    if (debug)
      std::cout << "Image saved to: " << inputPath << std::endl;

    cv::Mat image = cv::imread(inputPath, cv::IMREAD_COLOR);
    if (image.empty())
    {
      std::cerr << "Error loading image: " << inputPath << std::endl;
      return -1;
    }
    if (debug)
      std::cout << "Image loaded successfully.\n";

    cv::Mat correctedImage = adjustPerspective(image, perspectivePath, debug);
    cv::Mat thresholdImage = applyThresholding(correctedImage, thresholdPath, debug);
    cv::Mat floodFillImage = applyEdgeFloodFill(thresholdImage, floodFillPath, debug);
    cv::Mat processedImage = applyMorphology(floodFillImage, morphologyPath, debug);

    float value = performOCR(morphologyPath, debug);

    handleNotify(value, debug);

    int key;
    if (debug)
    {
      std::cout << "Image processing complete.\n";
      key = cv::waitKey(0);
    }
    else
    {
      key = cv::waitKey(10000);
    }

    if (key == 27)
      break;
  }

  return 0;
}