//
// Created by Konst on 22.09.2021.
//
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "src/view/attributeInfo.h"
#include "shaderSource.h"

class BaseShader {

protected:

    static GLuint compile(GLenum shaderType, const char* shaderText);
    static GLuint link(GLuint vertexShader, GLuint fragmentShader);
    static GLint* createArray(uint8_t size);

    template<typename T>
    static void setAttr(GLuint attributeId, const std::vector<T>& points, AttributeInfo info) {
        setAttr(attributeId, points.data(), info);
    }
    static void setAttr(GLuint attributeId, const void* points, AttributeInfo info);
    static void setAttr(GLuint attributeId, AttributeInfo info);

    static void set1(GLint uniformId, int value);
    static void set1(GLint uniformId, float value);
    static void set2(GLint uniformId, const glm::vec2& value);
    static void set2(GLint uniformId, const glm::vec2* value);
    static void set3(GLint uniformId, const float* value);
    static void set3(GLint uniformId, const glm::vec3& value);
    static void set3(GLint uniformId, const glm::vec3* value);
    static void set3(GLint uniformId, const glm::mat3& value);
    static void set3(GLint uniformId, const glm::mat3* value);
    static void set4(GLint uniformId, const float* value);
    static void set4(GLint uniformId, const glm::vec4& value);
    static void set4(GLint uniformId, const glm::vec4* value);
    static void set4(GLint uniformId, const glm::mat4& value);
    static void set4(GLint uniformId, const glm::mat4* value);


    GLuint programId;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLint* a; //attributes
    GLint* u; //uniforms
    uint8_t attributeCount;
    uint8_t uniformCount;

    GLint attr(const char* name) const;
    GLint uniform(const char* name) const;
    virtual void init() { }
    virtual void bind() { }
    virtual void unbind() { }
public:

    explicit BaseShader(uint8_t attributes, uint8_t uniforms);
    virtual ~BaseShader();

    void create(const ShaderSource& source);
    void destroy();
    void enable();
    void disable();
};


