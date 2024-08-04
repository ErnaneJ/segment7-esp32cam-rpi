cmake_minimum_required(VERSION 3.10)
project(Segment7Esp32camRpi)

# Define the path to Tesseract installed with Homebrew
set(Tesseract_INCLUDE_DIRS "/opt/homebrew/include")
set(Tesseract_LIBRARIES "/opt/homebrew/lib/libtesseract.dylib")

# Find OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Find CURL
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

add_executable(main
  src/main.cpp
  src/image_processor.cpp
  src/ocr_processor.cpp
  src/notifier.cpp
  src/image_downloader.cpp
  src/helpers.cpp
)

include_directories(${Tesseract_INCLUDE_DIRS})
include_directories("./include")

target_link_libraries(main
  ${OpenCV_LIBS}
  ${Tesseract_LIBRARIES}
  ${CURL_LIBRARIES}
)
