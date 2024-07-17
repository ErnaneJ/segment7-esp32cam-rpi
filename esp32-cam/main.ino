/**
 * @file main.cpp
 * @brief ESP32-CAM Web Server to serve captured images from the camera.
 */

#include <WebServer.h>
#include <WiFi.h>
#include <esp32cam.h>

// WiFi credentials

const char *WIFI_SSID = "NPITI-IoT";
const char *WIFI_PASS = "NPITI-IoT";

WebServer server(80);

// Default resolution for image capture
static auto resolution = esp32cam::Resolution::find(1024, 768);

/**
 * @brief Function to serve a captured JPEG image.
 * 
 * @param frame Unique pointer to the captured frame.
 */
void serve_jpg(std::unique_ptr<esp32cam::Frame> frame)
{
  Serial.println("Setting content to serve image");

  server.setContentLength(frame->size());
  server.send(200, "image/jpeg");
  WiFiClient client = server.client();
  frame->writeTo(client);
  Serial.println("Image served successfully");
}

/**
 * @brief Function to handle the request to capture and send a JPEG image.
 */
void handle_jpg()
{
  Serial.println("Changing resolution");

  if (!esp32cam::Camera.changeResolution(resolution))
    Serial.println("Failed to set resolution");
  
  Serial.println("Resolution set successfully");

  Serial.println("Capturing frame");
  auto frame = esp32cam::capture();
  if (frame == nullptr)
  {
    Serial.println("Failed to capture frame");
    server.send(503, "", "");
    return;
  }
  Serial.printf("Capture successful: %dx%d, size: %d bytes\n", frame->getWidth(), frame->getHeight(),
                static_cast<int>(frame->size()));

  if (!frame->isJpeg())
  {
    if (!frame->toJpeg(80)) // JPEG quality of 80%
    {
      Serial.println("Failed to convert to JPEG");
      server.send(503, "", "");
      return;
    }
  }

  // Serve the JPEG image
  server.setContentLength(frame->size());
  server.send(200, "image/jpeg");
  WiFiClient client = server.client();
  frame->writeTo(client);
  Serial.println("JPEG served successfully");
}

void setup()
{
  Serial.begin(115200);
  Serial.println();
  
  {
    using namespace esp32cam;
    Config cfg;
    cfg.setPins(pins::AiThinker);
    cfg.setResolution(Resolution::find(800, 600)); // Initial resolution setting
    cfg.setBufferCount(2);
    cfg.setJpeg(80);

    bool camera_ok = Camera.begin(cfg);
    Serial.println(camera_ok ? "Camera initialized" : "Failed to initialize camera");
  }

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  Serial.print("http://");
  Serial.println(WiFi.localIP());
  Serial.println("/cam.jpg");

  server.on("/cam.jpg", handle_jpg);

  server.begin();
}

void loop()
{
  server.handleClient();
}
