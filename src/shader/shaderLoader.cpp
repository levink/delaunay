#include <iostream>
#include "shader/shaderLoader.h"
#include "shader/shaderLog.h"

std::string ShaderLoader::getShaderText(const char* fileName) {
    std::string result;
    std::ifstream input(fileName, std::ifstream::binary);
    if (!input) {
        ShaderLog::warn("Could not open file", fileName);
        return result;
    }

    input.seekg(0, std::ios::end);
    size_t file_length = input.tellg();
    input.seekg(0, std::ios::beg);

    char* buf = new char[file_length + 1];
    std::streamsize readCount = 0;
    while (readCount < file_length) {
        input.read(buf + readCount, file_length - readCount);
        readCount += input.gcount();
    }

    buf[file_length] = 0;
    input.close();

    result.assign(buf);
    delete[] buf;

    return result;
}
ShaderSource ShaderLoader::load(const char* path) const {
    std::string text = getShaderText(path);

    static const auto vertexTag = "//#vertex";
    auto vertexPosition = text.find(vertexTag);
    if (vertexPosition == std::string::npos) {
        ShaderLog::warn("Vertex tag not found in shader", path);
        return ShaderSource();
    }

    static const auto fragmentTag = "//#fragment";
    auto fragmentPosition = text.find(fragmentTag);
    if (fragmentPosition == std::string::npos) {
        ShaderLog::warn("Fragment tag not found in shader", path);
        return ShaderSource();
    }

    auto uniforms = text.substr(0, vertexPosition);
    auto vertex = text.substr(0, fragmentPosition);
    auto fragment = uniforms + text.substr(fragmentPosition);

    ShaderSource result;
    result.vertex = vertex;
    result.fragment = fragment;
    return result;
}
