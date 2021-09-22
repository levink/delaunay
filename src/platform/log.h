#pragma once
#include <string_view>
class Log {
public:
    static void warn(std::string_view error);
    static void warn(std::string_view error, std::string_view description);
};