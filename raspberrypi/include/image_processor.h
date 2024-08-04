#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include "helpers.h"
#include <string>
#include <opencv2/opencv.hpp>
#include <ctime>
#include <sstream>

#define SUFFIX_THRESHOLD_IMAGE_PATH "_threshold.jpg"
#define SUFFIX_FLOOD_FILL_IMAGE_PATH "_flood_fill.jpg"
#define SUFFIX_MORPHOLOGY_IMAGE_PATH "_morphology.jpg"
#define SUFFIX_PERSPECTIVE_IMAGE_PATH "_perspective.jpg"
#define SUFFIX_INPUT_IMAGE_PATH "_ocr.txt"

cv::Mat adjustPerspective(cv::Mat image, const std::string &inputImage, bool debug);
cv::Mat applyThresholding(const cv::Mat &inputImage, const std::string &outputPath, bool debug);
cv::Mat applyEdgeFloodFill(const cv::Mat &inputImage, const std::string &outputPath, bool debug);
cv::Mat applyMorphology(const cv::Mat &inputImage, const std::string &outputPath, bool debug);

std::string generateFilePath(const std::string &prefix, const std::string &suffix, bool debug);

#endif // IMAGE_PROCESSOR_H
