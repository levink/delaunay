#pragma once
#include <string_view>

namespace ShaderLog {
	void warn(std::string_view error);
	void warn(std::string_view error, std::string_view data);
}