#pragma once
#include <glad/glad.h>
#include "shaderSource.h"

struct GLSLType {
    GLint size;
    GLenum type;
};

struct Attribute {
    GLint id;
    GLint size;
    GLenum type;
    std::string name;
    Attribute();
    explicit Attribute(const GLSLType& info, std::string name);
};

struct Uniform {
    GLint id;
    std::string name;
    Uniform();
    explicit Uniform(std::string name);
};

class Shader {

protected:

    constexpr static const GLSLType FLOAT = GLSLType {1, GL_FLOAT};
    constexpr static const GLSLType VEC_2 = GLSLType {2, GL_FLOAT};
    constexpr static const GLSLType VEC_3 = GLSLType {3, GL_FLOAT};

    static GLuint compile(GLenum shaderType, const char* shaderText);
    static GLuint link(GLuint vertexShader, GLuint fragmentShader);

    static void set3(const Uniform& uniform, const float* value);
    static void set4(const Uniform& uniform, const glm::mat4& value);

    template <typename T>
    static void attr(const Attribute& attribute, const std::vector<T>& items, GLsizei stride, GLuint offset){
        attr(attribute, items.data(), stride, offset);
    }
    static void attr(const Attribute& attribute, const void *data, GLsizei stride, GLuint offset);

//    static void set1(GLint uniformId, int value);
//    static void set1(GLint uniformId, float value);
//    static void set2(GLint uniformId, const glm::vec2& value);
//    static void set2(GLint uniformId, const glm::vec2* value);
//    static void set3(GLint uniformId, const float* value);
//    static void set3(GLint uniformId, const glm::vec3& value);
//    static void set3(GLint uniformId, const glm::vec3* value);
//    static void set3(GLint uniformId, const glm::mat3& value);
//    static void set3(GLint uniformId, const glm::mat3* value);
//    static void set4(GLint uniformId, const float* value);
//    static void set4(GLint uniformId, const glm::vec4& value);
//    static void set4(GLint uniformId, const glm::vec4* value);
//    static void set4(GLint uniformId, const glm::mat4& value);
//    static void set4(GLint uniformId, const glm::mat4* value);


    GLuint programId;
    GLuint vertexShader;
    GLuint fragmentShader;
    std::vector<Attribute> a;
    std::vector<Uniform> u;
public:

    explicit Shader(uint8_t uniforms, uint8_t attributes);
    virtual ~Shader();

    void create(const ShaderSource& source);
    void destroy();
    virtual void enable();
    virtual void disable();
};


