# ESP32-CAM Web Server

This file demonstrates a simple web server running on ESP32-CAM to serve captured JPEG images from the camera module.

## Features

- Captures JPEG images from ESP32-CAM.
- Serves captured images over HTTP.
- Supports changing image resolution dynamically.

## Hardware Requirements

- ESP32-CAM module
- USB to UART converter for programming
- Stable 5V power supply

## Software Requirements

- Arduino IDE with ESP32 board support
- ESP32 libraries:
  - [WebServer](https://github.com/espressif/arduino-esp32/tree/master/libraries/WebServer)
  - [WiFi](https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi)
  - [esp32cam](https://github.com/espressif/arduino-esp32/tree/master/libraries/esp32cam)

## Installation

1. Clone or download the repository.
2. Open `main.ino` in Arduino IDE.
3. Install the required libraries if not already installed.
4. Update `WIFI_SSID` and `WIFI_PASS` with your WiFi credentials.
5. Upload the sketch to your ESP32-CAM module.

## Usage

1. Connect ESP32-CAM to a power source and wait for it to connect to your WiFi network.
2. Open Serial Monitor in Arduino IDE to view the assigned IP address.
3. Access the camera feed using the IP address: `http://<ESP32-CAM_IP>/cam.jpg`.

## Configuration

- You can modify the initial camera resolution in the `setup()` function (`cfg.setResolution()`).
- Adjust the JPEG quality in `handle_jpg()` function (`frame->toJpeg()`).

## Troubleshooting

- Ensure ESP32-CAM is powered correctly and connected to WiFi.
- Check Serial Monitor for debug messages to diagnose any issues.
- Verify that the camera initializes correctly (`Camera.begin()`).
