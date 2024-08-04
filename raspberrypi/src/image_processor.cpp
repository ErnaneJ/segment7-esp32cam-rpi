#include "image_processor.h"

std::string generateFilePath(const std::string &prefix, const std::string &suffix, bool debug)
{
  if (debug)
  {
    return prefix + getCurrentDateTime("%Y_%m_%d_%H_%M_%S") + suffix;
  }
  else
  {
    return prefix + suffix;
  }
}

cv::Mat adjustPerspective(cv::Mat image, const std::string &inputImage, bool debug)
{
  std::vector<cv::Point2f> srcPoints;
  srcPoints.push_back(cv::Point2f(76, 160));  // upper left
  srcPoints.push_back(cv::Point2f(705, 114)); // upper right
  srcPoints.push_back(cv::Point2f(727, 514)); // lower right
  srcPoints.push_back(cv::Point2f(57, 590));  // lower left

  int largura = cv::max(cv::norm(srcPoints[0] - srcPoints[1]),
                        cv::norm(srcPoints[2] - srcPoints[3]));
  int altura = cv::max(cv::norm(srcPoints[1] - srcPoints[2]),
                       cv::norm(srcPoints[3] - srcPoints[0]));

  std::vector<cv::Point2f> dstPoints;
  dstPoints.push_back(cv::Point2f(0, 0));
  dstPoints.push_back(cv::Point2f(largura, 0));
  dstPoints.push_back(cv::Point2f(largura, altura));
  dstPoints.push_back(cv::Point2f(0, altura));

  cv::Mat perspectiveMatrix = cv::getPerspectiveTransform(srcPoints, dstPoints);
  cv::Mat correctedImage;
  cv::warpPerspective(image, correctedImage, perspectiveMatrix, cv::Size(largura, altura));

  cv::imwrite(generateFilePath("../assets/", SUFFIX_PERSPECTIVE_IMAGE_PATH, debug), correctedImage);
  if (debug)
    cv::imshow(generateFilePath("../assets/", SUFFIX_PERSPECTIVE_IMAGE_PATH, debug), correctedImage);

  return correctedImage;
}

cv::Mat applyThresholding(const cv::Mat &inputImage, const std::string &outputPath, bool debug)
{
  if (debug)
    std::cout << "Applying thresholding...\n";
  cv::Mat thresholdImage;
  cv::cvtColor(inputImage, thresholdImage, cv::COLOR_BGR2GRAY);
  cv::threshold(thresholdImage, thresholdImage, 110, 255, cv::THRESH_BINARY);
  cv::imwrite(outputPath, thresholdImage);
  if (debug)
    cv::imshow(outputPath, thresholdImage);
  return thresholdImage;
}

cv::Mat applyEdgeFloodFill(const cv::Mat &inputImage, const std::string &outputPath, bool debug)
{
  cv::Mat floodFillImage = inputImage.clone();

  for (int row = 0; row < floodFillImage.rows; row++)
  {
    for (int column = 0; column < floodFillImage.cols; column++)
    {
      if (row == 0 || column == 0 || row == floodFillImage.rows - 1 || column == floodFillImage.cols - 1)
      {
        if (floodFillImage.at<uchar>(row, column) == 0)
        {
          cv::floodFill(floodFillImage, cv::Point(column, row), 255);
        }
      }
    }
  }

  int borderSize = 250;
  cv::copyMakeBorder(floodFillImage, floodFillImage, borderSize, borderSize, borderSize, borderSize, cv::BORDER_CONSTANT, cv::Scalar(255, 0, 255));

  cv::imwrite(outputPath, floodFillImage);
  if (debug)
    cv::imshow(outputPath, floodFillImage);

  return floodFillImage;
}

cv::Mat applyMorphology(const cv::Mat &inputImage, const std::string &outputPath, bool debug)
{
  std::cout << "Applying morphological operations...\n";
  cv::Mat image = inputImage.clone();
  cv::Mat structuringVerticalElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 5));
  cv::Mat structuringHorizontalElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 1));
  cv::Mat structuringElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));

  cv::morphologyEx(image, image, cv::MORPH_OPEN, structuringElement);
  cv::morphologyEx(image, image, cv::MORPH_CLOSE, structuringElement);

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

  cv::erode(image, image, structuringHorizontalElement);
  cv::erode(image, image, structuringHorizontalElement);
  cv::erode(image, image, structuringHorizontalElement);
  cv::dilate(image, image, structuringHorizontalElement);
  cv::dilate(image, image, structuringHorizontalElement);

  cv::dilate(image, image, structuringVerticalElement);
  cv::dilate(image, image, structuringVerticalElement);
  cv::dilate(image, image, structuringVerticalElement);

  // cv::imshow(outputPath, image);
  cv::imwrite(outputPath, image);
  std::cout << "Morphological processing complete.\n";
  return image;
}