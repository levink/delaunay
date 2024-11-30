#include "shaderSource.h"

namespace util {
    static void swap(ShaderSource& left, ShaderSource& right) {
        std::swap(left.vertex, right.vertex);
        std::swap(left.fragment, right.fragment);
    }
}

ShaderSource::ShaderSource(const ShaderSource &right) {
    vertex = right.vertex;
    fragment = right.fragment;
}
ShaderSource::ShaderSource(ShaderSource &&right) noexcept : ShaderSource() {
    util::swap(*this, right);
}
ShaderSource &ShaderSource::operator=(const ShaderSource &right) {
    if (this == &right) {
        return *this;
    }

    vertex = right.vertex;
    fragment = right.fragment;
    return *this;
}
ShaderSource &ShaderSource::operator=(ShaderSource &&right) noexcept {
    if (this == &right) {
        return *this;
    }

    util::swap(*this, right);
    return *this;
}