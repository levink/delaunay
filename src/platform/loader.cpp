#include "loader.h"
#include <iostream>
#include <fstream>

std::string ShaderLoader::getShaderText(const char* fileName) {
    std::string result;
    std::ifstream input(fileName, std::ifstream::binary);
    if (!input) {
        std::wcout << L"Error: could not open file: " << fileName << std::endl;
        return result;
    }

    input.seekg(0, std::ios::end);
    std::streamoff file_length = input.tellg();
    input.seekg(0, std::ios::beg);

    auto buf_size = static_cast<size_t>(file_length + 1);
    char* buf = new char[buf_size];
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
ShaderSource ShaderLoader::load(const char* path) {
    static const auto vertexTag = "//#vertex";
    static const auto fragmentTag = "//#fragment";

    std::string text = getShaderText(path);
    auto vertexPosition = text.find(vertexTag);
    auto fragmentPosition = text.find(fragmentTag);

    auto uniforms = text.substr(0, vertexPosition);
    auto vertex = text.substr(0, fragmentPosition);
    auto fragment = uniforms + text.substr(fragmentPosition);

    ShaderSource result;
    result.vertex = vertex;
    result.fragment = fragment;
    return result;
}
