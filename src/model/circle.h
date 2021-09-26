#pragma once
#include "src/pch.h"
#include "face.h"

struct CircleVertex {
    float position[2];
    float center[2];
    float radius;
    float fill;
    CircleVertex();
    CircleVertex(float x, float y, float cx, float cy, float r, float fill);
};

struct CircleModel {
    CircleVertex vertex[4];
    Face face[2];
    CircleModel(float x, float y, float r, bool filled);
};
