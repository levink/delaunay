#include <iostream>
#include "shader/shaderLog.h"

void ShaderLog::warn(std::string_view error) {
	std::cout << "[Warning] " << error << std::endl;
}
void ShaderLog::warn(std::string_view error, std::string_view data) {
	std::cout << "[Warning] " << error << " " << data << std::endl;
}