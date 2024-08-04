#include "helpers.h"

std::string getCurrentDateTime(const std::string &format)
{
    std::time_t now = std::time(nullptr);
    std::tm *ltm = std::localtime(&now);

    char buffer[100];
    std::strftime(buffer, sizeof(buffer), format.c_str(), ltm);

    return std::string(buffer);
}