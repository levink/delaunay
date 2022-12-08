#pragma once
#include "pch.h"
class Log {
public:
    static void out(std::string_view message);
    static void warn(std::string_view error);
    static void warn(std::string_view error, std::string_view description);
};