#include "Shader.hpp"

#include <fmt/core.h>

#include <fstream>

static inline std::string_view parentOf(const std::string_view & path) {
    auto lastSlash = path.find_last_of('/');
    if (lastSlash == std::string_view::npos)
        lastSlash = 0;
    return path.substr(0, lastSlash);
}

static std::string shaderSource(const std::string_view & path,
                                std::vector<std::string> & loaded) {
    for (auto & l : loaded) {
        if (l == path) {
            fmt::print("File already included {}, skipping\n", path);
            return "";
        }
    }

    loaded.emplace_back(path);

    std::ifstream is(path.data());
    if (!is) {
        fmt::print("shaderSource path={} failed to open file\n", path);
        return std::string();
    }

    std::string source;

    std::string line;
    for (; std::getline(is, line);) {
        if (line[0] == '#') {
            auto space = line.find_first_of(' ');
            if (line.substr(0, space) == "#include") {
                auto includeFile =
                    line.substr(space + 2, line.length() - space - 3);
                auto parent = std::string(parentOf(path));
                line = shaderSource(parent + '/' + includeFile, loaded);
            }
        }
        source += line + '\n';
    }

    return source;
}

static std::string shaderSource(const std::string_view & path) {
    std::vector<std::string> loaded;
    return shaderSource(path, loaded);
}

static bool compileSuccess(GLuint shader) {
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    return success != GL_FALSE;
}

static bool linkSuccess(GLuint program) {
    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    return success != GL_FALSE;
}

static std::string compileError(GLuint shader) {
    GLint logSize = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);

    std::vector<GLchar> errorLog(logSize);
    glGetShaderInfoLog(shader, logSize, &logSize, &errorLog[0]);

    return std::string(errorLog.begin(), errorLog.end());
}

static std::string linkError(GLuint program) {
    GLint logSize = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);

    std::vector<GLchar> errorLog(logSize);
    glGetProgramInfoLog(program, logSize, &logSize, &errorLog[0]);

    return std::string(errorLog.begin(), errorLog.end());
}

static GLuint compileShader(GLuint shaderType,
                            const std::string_view & shaderSource) {
    GLuint shader = glCreateShader(shaderType);
    fmt::print("Compiling shader {}: type={}\n", shader, shaderType);
    const char * source = shaderSource.data();
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    return shader;
}

Shader::Shader() : program(glCreateProgram()) {
    fmt::print("Created shader program {}\n", program);
}

Shader::~Shader() {
    fmt::print("Deleting shader program {}\n", program);
    glDeleteProgram(program);
}

bool Shader::loadFromSource(const std::string_view & vertexSource,
                            const std::string_view & fragmentSource) {
    GLuint vShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    if (!compileSuccess(vShader)) {
        fmt::print("failed to compile vertex shader {}: {}\n", vShader,
                   compileError(vShader));
        glDeleteShader(vShader);
        return false;
    }

    GLuint fShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!compileSuccess(fShader)) {
        fmt::print("failed to compile fragment shader {}: {}\n", fShader,
                   compileError(fShader));
        glDeleteShader(vShader);
        glDeleteShader(fShader);
        return false;
    }

    glAttachShader(program, vShader);
    glAttachShader(program, fShader);

    glLinkProgram(program);

    glDetachShader(program, vShader);
    glDetachShader(program, fShader);
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    if (!linkSuccess(program)) {
        fmt::print("failed to link shader program {}: {}\n", program,
                   linkError(program));
        return false;
    }

    fmt::print("Shader program {} was compiled and linked\n", program);
    return true;
}

bool Shader::loadFromPath(const std::string_view & vertexPath,
                          const std::string_view & fragmentPath) {
    fmt::print("loading Shader from paths vertex=\"{}\" fragment=\"{}\"\n",
               vertexPath, fragmentPath);

    std::string vertexSource = shaderSource(vertexPath);
    std::string fragmentSource = shaderSource(fragmentPath);

    if (vertexSource.empty()) {
        fmt::print("vertex shader source could not be loaded from path {}\n",
                   vertexPath);
        return false;
    }

    if (fragmentSource.empty()) {
        fmt::print("fragment shader source could not be loaded from path {}\n",
                   fragmentPath);
        return false;
    }

    return loadFromSource(vertexSource, fragmentSource);
}

GLuint Shader::getProgram() const {
    return program;
}

void Shader::bind() const {
    glUseProgram(program);
}

void Shader::unbind() const {
    glUseProgram(0);
}

GLuint Shader::uniformLocation(const std::string_view & name) const {
    return glGetUniformLocation(program, name.data());
}

void Shader::setBool(const std::string_view & name, bool value) const {
    setBool(uniformLocation(name), value);
}

void Shader::setBool(GLuint location, bool value) const {
    glUniform1i(location, static_cast<int>(value));
}

void Shader::setInt(const std::string_view & name, int value) const {
    setInt(uniformLocation(name), value);
}

void Shader::setInt(GLuint location, int value) const {
    glUniform1i(location, value);
}

void Shader::setFloat(const std::string_view & name, float value) const {
    setFloat(uniformLocation(name), value);
}

void Shader::setFloat(GLuint location, float value) const {
    glUniform1f(location, value);
}

void Shader::setVec2(const std::string_view & name, const glm::vec2 & value) const {
    setVec2(uniformLocation(name), value);
}

void Shader::setVec2(GLuint location, const glm::vec2 & value) const {
    glUniform2fv(location, 1, &value.x);
}

void Shader::setVec3(const std::string_view & name, const glm::vec3 & value) const {
    setVec3(uniformLocation(name), value);
}

void Shader::setVec3(GLuint location, const glm::vec3 & value) const {
    glUniform3fv(location, 1, &value.x);
}

void Shader::setVec4(const std::string_view & name, const glm::vec4 & value) const {
    setVec4(uniformLocation(name), value);
}

void Shader::setVec4(GLuint location, const glm::vec4 & value) const {
    glUniform4fv(location, 1, &value.x);
}

void Shader::setMat2(const std::string_view & name, const glm::mat2 & value) const {
    setMat2(uniformLocation(name), value);
}

void Shader::setMat2(GLuint location, const glm::mat2 & value) const {
    glUniformMatrix2fv(location, 1, GL_FALSE, &value[0][0]);
}

void Shader::setMat3(const std::string_view & name, const glm::mat3 & value) const {
    setMat3(uniformLocation(name), value);
}

void Shader::setMat3(GLuint location, const glm::mat3 & value) const {
    glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
}

void Shader::setMat4(const std::string_view & name, const glm::mat4 & value) const {
    setMat4(uniformLocation(name), value);
}

void Shader::setMat4(GLuint location, const glm::mat4 & value) const {
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

static const std ::string defaultVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;
out vec3 FragPos;
out vec3 FragNorm;
out vec2 FragTex;
uniform mat4 mvp;
uniform mat4 model;
void main() {
    gl_Position = mvp * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    FragNorm = aNorm;
    FragTex = aTex;
})";

static const std ::string defaultFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
uniform sampler2D gTexture;
in vec3 FragPos;
in vec3 FragNorm;
in vec2 FragTex;
void main() {
    FragColor = texture(gTexture, FragTex);
})";

Shader::Ptr Shader::defaultShader() {
    fmt::print("Loading the default shader\n");
    auto shader = std::make_shared<Shader>();
    if (shader
        && !shader->loadFromSource(defaultVertexShaderSource,
                                   defaultFragmentShaderSource)) {
        fmt::print("Failed to compile the default shader\n");
        return nullptr;
    }
    return shader;
}

Shader::Ptr Shader::fromFragmentPath(const std::string_view & path) {
    auto source = shaderSource(path);
    return fromFragmentSource(source);
}

Shader::Ptr Shader::fromFragmentSource(const std::string_view & fragmentSource) {
    fmt::print("Loading a fragment shader with default vertex shader\n");
    auto shader = std::make_shared<Shader>();
    if (shader
        && !shader->loadFromSource(defaultVertexShaderSource, fragmentSource)) {
        fmt::print("Failed to compile the fragment shader from source\n");
        return nullptr;
    }
    return shader;
}
