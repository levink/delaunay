//
// Created by Konst on 22.09.2021.
//
#pragma once
#include <glm/glm.hpp>
#include <string>
#include <stdexcept>

struct Color {
    static glm::vec3 asphalt;

private:
    static glm::vec3 parse(const std::string& color);
    static float parseHex(const std::string& colorComponent);
};
