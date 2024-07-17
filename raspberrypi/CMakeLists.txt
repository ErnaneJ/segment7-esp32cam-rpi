cmake_minimum_required(VERSION 3.0.0)
project(main VERSION 0.1.0 LANGUAGES C CXX)

# Definir o caminho para o Tesseract instalado via Homebrew
set(Tesseract_INCLUDE_DIRS "/opt/homebrew/include")
set(Tesseract_LIBRARIES "/opt/homebrew/lib/libtesseract.dylib")

# Encontrar o pacote OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Incluir diretórios de inclusão do Tesseract
include_directories(${Tesseract_INCLUDE_DIRS})

# Encontrar e incluir o pacote libcurl
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

# Configurar o executável e linkar com as bibliotecas necessárias
add_executable(main main.cpp)
target_link_libraries(main ${OpenCV_LIBS} ${Tesseract_LIBRARIES} ${CURL_LIBRARIES})
