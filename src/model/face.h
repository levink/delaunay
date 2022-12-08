#pragma once
#include "pch.h"

struct Face {
    uint16_t a, b, c;
    Face();
    Face(uint16_t a, uint16_t b, uint16_t c);
};