//
// Created by Konst on 22.09.2021.
//
#pragma once
#include "src/pch.h"


struct Face {
    uint16_t a, b, c;
    Face(uint16_t a, uint16_t b, uint16_t c);
};

struct CircleVertex {
    float position[2];
    float center[2];
    float radius;
    float color[3];
};

struct CircleModel {
    std::vector<CircleVertex> vertices;
    std::vector<Face> faces;
    CircleModel() = default;
    CircleModel(float x, float y, float r);
};
