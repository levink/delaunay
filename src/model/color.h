#pragma once
#include "pch.h"

struct Color {
    static glm::vec3 asphalt;
    static glm::vec3 teal;
    static glm::vec3 orange;
    static glm::vec3 yellow;

private:
    static glm::vec3 parse(const std::string& color);
    static float parseHex(const std::string& colorComponent);
};
