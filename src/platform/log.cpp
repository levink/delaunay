#include <iostream>
#include "log.h"

void Log::out(std::string_view message) {
    std::wcout << "[Out] " << message.data() << std::endl;
}

void Log::warn(std::string_view error) {
    std::wcout << "[Warn] " << error.data() << std::endl;
}

void Log::warn(std::string_view error, std::string_view description) {
    std::wcout << "[Warn] " << error.data() << ": " << description.data() << std::endl;
}