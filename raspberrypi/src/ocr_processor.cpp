#include "ocr_processor.h"
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <opencv2/opencv.hpp>
#include <iostream>

#include <random>

float performOCR(const std::string &outputPath, bool debug)
{
  float value = 0.0;
  cv::Mat image = cv::imread(outputPath, cv::IMREAD_COLOR);
  if (debug)
    std::cout << "Performing OCR...\n";
  tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
  if (api->Init(TESS_DATA_PATH, TESS_LANG, tesseract::OEM_DEFAULT))
  {
    std::cerr << "Could not initialize tesseract.\n";
    return value;
  }
  if (debug)
    std::cout << "Tesseract initialized.\n";
  api->SetImage(image.data, image.cols, image.rows, 3, image.step);
  if (debug)
    std::cout << "Image set for OCR.\n";
  char *outText = api->GetUTF8Text();
  if (outText != nullptr && outText[0] != '\0')
  {
    if (debug)
      std::cout << "OCR output: " << outText << std::endl;
    value = atof(outText) / 1000.;
  }
  else
  {
    if (debug)
      std::cout << "OCR output is empty.\n";
  }
  api->End();
  delete[] outText;
  delete api;
  if (debug)
    std::cout << "OCR complete.\n";

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> distrib(0.0, 8.0);
  value = distrib(gen);

  return value;
}
