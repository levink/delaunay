//
// Created by Konst on 22.09.2021.
//
#include <glm/gtc/type_ptr.hpp>
#include "src/platform/log.h"
#include "shaderBase.h"


BaseShader::BaseShader(uint8_t attributes, uint8_t uniforms):
    programId(0),
    vertexShader(0),
    fragmentShader(0),
    attributeCount(attributes),
    uniformCount(uniforms) {

    a = createArray(attributeCount);
    u = createArray(uniformCount);
}
BaseShader::~BaseShader() {
    destroy();
    delete[] a;
    delete[] u;
}
void BaseShader::destroy() {
    if (programId > 0) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(programId);
        programId = 0;
        vertexShader = 0;
        fragmentShader = 0;
    }
}
void BaseShader::create(const ShaderSource &source) {
    vertexShader = compile(GL_VERTEX_SHADER, source.vertex.c_str());
    fragmentShader = compile(GL_FRAGMENT_SHADER, source.fragment.c_str());
    programId = link(vertexShader, fragmentShader);
    if (!programId) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        vertexShader = 0;
        fragmentShader = 0;
    }
    init();
}
GLuint BaseShader::compile(GLenum shaderType, const char *shaderText) {
    if (shaderText == nullptr) {
        std::string error;
        switch (shaderType) {
            case GL_VERTEX_SHADER: error = "Vertex shader text is null"; break;
            case GL_FRAGMENT_SHADER: error = "Fragment shader text is null"; break;
            default: error = "Shader text is null"; break;
        }
        Log::warn(error);
        return 0;
    }

    GLint compiled = 0;
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderText, nullptr);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled) {
        return shader;
    }

    GLint length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    if (length) {
        std::vector<GLchar> description(length);
        glGetShaderInfoLog(shader, length, &length, description.data());

        std::string error;
        switch (shaderType) {
            case GL_VERTEX_SHADER: error = "Could not compile vertex shader"; break;
            case GL_FRAGMENT_SHADER: error = "Could not compile fragment shader"; break;
            default: error = "Could not compile shader"; break;
        }
        Log::warn(error, description.data());
    }
    else {
        Log::warn("Could not compile shader, but info log is empty");
    }

    glDeleteShader(shader);
    return 0;
}
GLuint BaseShader::link(GLuint vertexShader, GLuint fragmentShader) {
    if (!vertexShader || !fragmentShader) {
        Log::warn("Can't link shader because of empty parts");
        return 0;
    }

    GLint linked = 0;
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (linked) {
        glDetachShader(program, vertexShader);
        glDetachShader(program, fragmentShader);
        return program;
    }

    GLint length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    if (length) {
        std::vector<GLchar> info(length);
        glGetProgramInfoLog(program, length, &length, info.data());
        Log::warn("Could not link program", info.data());
    }
    else {
        Log::warn("Could not link shader, but info log is empty");
    }

    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteProgram(program);
    return 0;
}
GLint* BaseShader::createArray(uint8_t size) {
    if (size > 0)
        return new GLint[size];
    return nullptr;
}
void BaseShader::enable() {
    glUseProgram(programId);
}
void BaseShader::disable() {
    for (int i = 0; i < attributeCount; i++) {
        glDisableVertexAttribArray(a[i]);
    }
    glUseProgram(0);
}
GLint BaseShader::attr(const char* name) const {
    return glGetAttribLocation(programId, name);
}
GLint BaseShader::uniform(const char* name) const {
    return glGetUniformLocation(programId, name);
}

void BaseShader::set1(const GLint uniformId, const int value) {
    if (uniformId != -1)
        glUniform1i(uniformId, value);
}
void BaseShader::set1(const GLint uniformId, const float value) {
    if (uniformId != -1)
        glUniform1f(uniformId, value);
}
void BaseShader::set2(const GLint uniformId, const glm::vec2& value) {
    if (uniformId != -1)
        glUniform2fv(uniformId, 1, value_ptr(value));
}
void BaseShader::set2(const GLint uniformId, const glm::vec2 * value)
{
    if (uniformId != -1)
        glUniform2fv(uniformId, 1, (GLfloat *)value);
}
void BaseShader::set3(const GLint uniformId, const float* value) {
    if (uniformId != -1)
        glUniform3fv(uniformId, 1, value);
}
void BaseShader::set3(const GLint uniformId, const glm::vec3 * value) {
    if (uniformId != -1)
        glUniform3fv(uniformId, 1, (GLfloat *)value);
}
void BaseShader::set3(const GLint uniformId, const glm::vec3 & value) {
    if (uniformId != -1)
        glUniform3fv(uniformId, 1, value_ptr(value));
}
void BaseShader::set3(const GLint uniformId, const glm::mat3 & value) {
    if (uniformId != -1)
        glUniformMatrix3fv(uniformId, 1, GL_FALSE, value_ptr(value));
}
void BaseShader::set3(const GLint uniformId, const glm::mat3 * value) {
    if (uniformId != -1)
        glUniformMatrix3fv(uniformId, 1, GL_FALSE, (GLfloat*)value);
}
void BaseShader::set4(const GLint uniformId, const float* value) {
    if (uniformId != -1)
        glUniform4fv(uniformId, 1, value);
}
void BaseShader::set4(const GLint uniformId, const glm::vec4 & value) {
    if (uniformId != -1)
        glUniform4fv(uniformId, 1, value_ptr(value));
}
void BaseShader::set4(const GLint uniformId, const glm::vec4 * value) {
    if (uniformId != -1)
        glUniform4fv(uniformId, 1, (GLfloat *)value);
}
void BaseShader::set4(const GLint uniformId, const glm::mat4 & value) {
    if (uniformId != -1)
        glUniformMatrix4fv(uniformId, 1, GL_FALSE, value_ptr(value));
}
void BaseShader::set4(const GLint uniformId, const glm::mat4 * value) {
    if (uniformId != -1)
        glUniformMatrix4fv(uniformId, 1, GL_FALSE, (GLfloat*)value);
}
void BaseShader::setAttr(GLuint attributeId, const void* data, AttributeInfo info) {
    glEnableVertexAttribArray(attributeId);
    glVertexAttribPointer(
            attributeId,
            info.dimension,
            info.dataType,
            GL_FALSE,
            info.elementSize,
            ((GLbyte*)data) + info.offsetBytes);
}
void BaseShader::setAttr(GLuint attributeId, AttributeInfo info) {
    glEnableVertexAttribArray(attributeId);
    glVertexAttribPointer(
            attributeId,
            info.dimension,
            info.dataType,
            GL_FALSE,
            info.elementSize,
            (void*)(static_cast<unsigned long>(info.offsetBytes)));
}
