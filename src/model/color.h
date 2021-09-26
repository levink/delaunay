#pragma once
#include "src/pch.h"

struct Color {
    static glm::vec3 asphalt;
    static glm::vec3 teal;
    static glm::vec3 orange;

private:
    static glm::vec3 parse(const std::string& color);
    static float parseHex(const std::string& colorComponent);
};
