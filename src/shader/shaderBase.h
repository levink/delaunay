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
    Attribute(GLSLType type, std::string name);
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
    static void attr(const Attribute& attribute, const void *data, GLsizei stride, GLuint offset);
    static void attr(const Attribute& attribute, GLsizei stride, GLuint offset);
    static void set1(const Uniform& uniform, float value);
    static void set3(const Uniform& uniform, const glm::vec3& value);
    static void set4(const Uniform& uniform, const glm::mat4& value);

    GLuint programId;
    GLuint vertexShader;
    GLuint fragmentShader;
    std::vector<Attribute> a;
    std::vector<Uniform> u;
public:

    explicit Shader(uint8_t uniforms, uint8_t attributes);
    virtual ~Shader();

    uint32_t getProgramId() const { return programId; }
    void create(const ShaderSource& source);
    void destroy();
    virtual void enable() const;
    virtual void disable() const;
};


