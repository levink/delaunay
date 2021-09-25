#pragma once
#include "src/pch.h"
#include "face.h"

struct LineVertex {
    float position[2];
};

struct LineModel {
    std::vector<LineVertex> vertices;
};

