#ifndef NOTIFIER_H
#define NOTIFIER_H

#include "helpers.h"
#include <iostream>
#include <curl/curl.h>

#define MULTIMETER_MINIMUM_VALUE 3.0
#define MULTIMETER_MAXIMUM_VALUE 5.0
#define ADAFRUIT_API_KEY "ADAFRUIT_API_KEY"
#define ADAFRUIT_URL "https://io.adafruit.com/api/v2/Ernane/feeds/segment7-esp32cam-rpi/data"

bool bublishAdafruit(float value, bool debug);
bool whatsNotify(const std::string &number, const std::string &message, bool debug);
bool handleNotify(float value, bool debug);

#endif // NOTIFIER_H
