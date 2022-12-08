#include "platform/log.h"
#include "shaderBase.h"

Attribute::Attribute(): id(-1), size(-1), type(GL_NONE) { }
Attribute::Attribute(GLSLType type, std::string name):
    id(-1),
    size(type.size),
    type(type.type),
    name(std::move(name)) { }

Uniform::Uniform(): id(-1) { }
Uniform::Uniform(std::string name):
    id(-1),
    name(std::move(name)) { }

Shader::Shader(uint8_t uniforms, uint8_t attributes):
    programId(0),
    vertexShader(0),
    fragmentShader(0) {
    u.resize(uniforms);
    a.resize(attributes);
}
Shader::~Shader() {
    destroy();
}
void Shader::destroy() {
    if (programId > 0) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(programId);
        programId = 0;
        vertexShader = 0;
        fragmentShader = 0;
    }
}
void Shader::create(const ShaderSource &source) {
    vertexShader = compile(GL_VERTEX_SHADER, source.vertex.c_str());
    fragmentShader = compile(GL_FRAGMENT_SHADER, source.fragment.c_str());
    programId = link(vertexShader, fragmentShader);
    if (!programId) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        vertexShader = 0;
        fragmentShader = 0;
        return;
    }

    glUseProgram(programId);
    for(auto& uniform : u) {
        uniform.id = glGetUniformLocation(programId, uniform.name.c_str());
    }
    for(auto& attribute : a) {
        attribute.id = glGetAttribLocation(programId, attribute.name.c_str());
    }
    glUseProgram(0);
}
GLuint Shader::compile(GLenum shaderType, const char *shaderText) {
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
GLuint Shader::link(GLuint vertexShader, GLuint fragmentShader) {
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
void Shader::enable() const {
    glUseProgram(programId);
    for (auto& attribute : a) {
        if (attribute.id != -1) {
            glEnableVertexAttribArray(attribute.id);
        }
    }
}
void Shader::disable() const {
    for (auto& attribute : a) {
        if (attribute.id != -1) {
            glDisableVertexAttribArray(attribute.id);
        }
    }
    glUseProgram(0);
}

void Shader::attr(const Attribute& attribute, const void *data, GLsizei stride, GLuint offset) {
    if (attribute.id != -1) {

        glVertexAttribPointer( 
            attribute.id, 
            attribute.size, 
            attribute.type, 
            GL_FALSE, 
            stride,
            static_cast<const GLbyte*>(data) + offset
        );
    }
}
void Shader::attr(const Attribute &attribute, GLsizei stride, GLuint offset) {
    if (attribute.id != -1) {
        glVertexAttribPointer(
            attribute.id, 
            attribute.size, 
            attribute.type, 
            GL_FALSE, 
            stride, 
            static_cast<const GLubyte*>(nullptr) + offset
        );
    }
}
void Shader::set1(const Uniform& uniform, float value) {
    if (uniform.id != -1) {
        glUniform1f(uniform.id, value);
    }
}
void Shader::set3(const Uniform& uniform, const glm::vec3& value) {
    if (uniform.id != -1) {
        glUniform3fv(uniform.id, 1, value_ptr(value));
    }
}
void Shader::set4(const Uniform& uniform, const glm::mat4& value) {
    if (uniform.id != -1) {
        glUniformMatrix4fv(uniform.id, 1, GL_FALSE, value_ptr(value));
    }
}




