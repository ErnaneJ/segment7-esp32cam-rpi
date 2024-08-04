#include "notifier.h"

bool bublishAdafruit(float value, bool debug)
{
  if (debug)
    std::cout << "Adafruit Publish received 🏗️" << std::endl;

  CURL *curl;
  CURLcode res;
  struct curl_slist *headers = NULL;

  curl = curl_easy_init();
  if (curl)
  {
    std::string postFields = "value=" + std::to_string(value);
    curl_easy_setopt(curl, CURLOPT_URL, ADAFRUIT_URL);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());
    headers = curl_slist_append(headers, ("X-AIO-Key: " + std::string(ADAFRUIT_API_KEY)).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
      std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
      return false;
    }
    else
    {
      std::cout << "Value published successfully." << std::endl;
      return true;
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
  }
  else
  {
    std::cerr << "Failed to initialize cURL." << std::endl;
    return false;
  }
}

bool whatsNotify(const std::string &number, const std::string &message, bool debug)
{
  if (debug)
    std::cout << "Whats Notify received 🏗️" << std::endl;

  CURL *curl;
  CURLcode res;
  curl = curl_easy_init();
  if (curl)
  {
    std::string url = "https://buddy.ernane.dev/api/v1/send-message/";
    std::string jsonBody = "{\"number\":\"" + number + "\",\"message\":\"" + message + "\",\"token\":\"3967f4a6-3cd3-4ded-b08e-3fcbf3dbf6a9\"}";

    std::cout << jsonBody;

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());

    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
      if (debug)
        std::cerr << "Erro ao enviar mensagem! 😢" << std::endl
                  << curl_easy_strerror(res) << std::endl;
      curl_easy_cleanup(curl);
      return false;
    }

    if (debug)
      std::cout << "Mensagem enviada com sucesso!" << std::endl;

    curl_easy_cleanup(curl);
    return true;
  }

  if (debug)
    std::cerr << "Erro ao inicializar CURL! 😢" << std::endl;

  return false;
}

bool handleNotify(float value, bool debug)
{
  std::cout << "Multimeter value: " << value << std::endl;
  bublishAdafruit(value, debug);
  if (value >= MULTIMETER_MINIMUM_VALUE && value <= MULTIMETER_MAXIMUM_VALUE)
  {
    std::cout << "Value is within the acceptable range." << std::endl;
    return true;
  }
  else
  {
    std::cout << std::endl;
    whatsNotify("5584992207080", "🚨 *Segment7-esp32cam-rpi Notification* 🚨\\n\\n> Value is outside the acceptable range!\\n\\n- Current Value: " + std::to_string(value) + "V" + "\\n- Acceptable range: [" + std::to_string(MULTIMETER_MINIMUM_VALUE) + "V - " + std::to_string(MULTIMETER_MAXIMUM_VALUE) + "V]\\n- Server Time: " + getCurrentDateTime("%d/%m/%Y %H:%M:%S") + "", debug);
    std::cout << std::endl
              << "Value is outside the acceptable range." << std::endl;
    return false;
  }

  return false;
}
