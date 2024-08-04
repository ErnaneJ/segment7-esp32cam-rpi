#ifndef OCR_PROCESSOR_H
#define OCR_PROCESSOR_H

#include <string>

#define TESS_DATA_PATH "../assets/tessdata"
#define TESS_LANG "7seg"

float performOCR(const std::string &imagePath, bool debug);

#endif // OCR_PROCESSOR_H
