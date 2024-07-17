# Image Download, Processing with OpenCV, and OCR using Tesseract

This project demonstrates downloading an image from a URL, processing it using OpenCV for morphology operations, and performing Optical Character Recognition (OCR) using Tesseract to extract text from the image.

## Requirements

- **OpenCV**: Computer vision library. Installation may vary depending on the operating system.
- **Tesseract**: OCR engine. Installation required with support for the desired language (in the example, "digits1" is being used).
- **CURL**: Library for data transfer via URL.
- **CMake**: For compiling the code, if necessary.
- **C++ Compiler**: Support for C++11 or higher.

## Installation and Compilation

1. **Install Dependencies**:
   - Install OpenCV following the instructions for your operating system.
   - Install Tesseract and the necessary training data packages (tessdata) for the desired language and configuration.

2. **Compilation**:
   ```bash
   cd build
   cmake ..
   make
   ```

## Execution

After successfully compiling the code:

```bash
./main
```

This command will:

- Download the image from the specified URL.
- Process the image with morphology operations using OpenCV.
- Perform OCR on the image to extract text using Tesseract.
- Display the recognized text in the console and save the processed image as `output.png` in `../assets/`.

### Notes

- Ensure to adjust the URL (`url`) and the path to save the image (`savePath`) in the code as needed.
- Ensure an active internet connection to download the image from the specified URL.