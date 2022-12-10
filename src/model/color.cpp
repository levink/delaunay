#include "color.h"

glm::vec3 Color::asphalt = Color::parse("424250");
glm::vec3 Color::teal = Color::parse("009989");
glm::vec3 Color::orange = Color::parse("DB5700");


glm::vec3 Color::parse(const std::string &color)  {
    if (color.size() != 6) {
        throw std::runtime_error("Bad format");
    }

    auto red = color.substr(0, 2);
    auto green = color.substr(2, 2);
    auto blue = color.substr(4, 2);

    const float r = parseHex(red);
    const float g = parseHex(green);
    const float b = parseHex(blue);

    return {r, g, b};
}
float Color::parseHex(const std::string &hex) {
    int value = std::stoi(hex, nullptr, 16);
    return static_cast<float>(value) / 255.f;
}
