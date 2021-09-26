//
// Created by Konst on 22.09.2021.
//
#pragma once
#include "src/pch.h"
#include "face.h"

struct CircleVertex {
    float position[2];
    float center[2];
    float radius;
    float fill;
    float color[3];
};

struct CircleModel {
    std::vector<CircleVertex> vertices;
    std::vector<Face> faces;
    CircleModel() = default;
    CircleModel(float x, float y, float r);
    void fill(bool value);
};
