#include <OpenGLProgram.hpp>
#include <Logging.hpp>

Shader::~Shader()
{
    glDeleteShader(glID); $gl_chk
}

bool Shader::Compile() const
{
    const char* sourceCharArr = this->source.c_str();
    glShaderSource(glID, 1, &sourceCharArr, NULL); $gl_chk
    glCompileShader(glID); $gl_chk
    int success;
    glGetShaderiv(glID, GL_COMPILE_STATUS, &success); $gl_chk
    if (!success) {
        std::string log(2048, '\0');
        glGetShaderInfoLog(glID, 2048, NULL, log.data()); $gl_chk
        spdlog::error("failed to compile {}:\n{}", label, log);
        return false;
    }
    this->compiled = success;
    return true;
}

bool Shader::Attach(GLuint programID) const
{
    glAttachShader(programID, glID); $gl_chk
    return true;
}

void Shader::SetSource(const std::string& src, bool compile)
{
    this->source = src;

    if (compile) {
        this->Compile();
    }
}

void Shader::SetSourceFromFile(const char* filePath, bool compile)
{
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if (!fileStream.is_open()) {
        spdlog::error("Could not read file {}.", filePath);
    }

    std::string line = "";
    while (!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    SetSource(content, compile);
}

OpenGLProgram::OpenGLProgram()
{
    programID = glCreateProgram(); $gl_chk
    glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w); $gl_chk
}

OpenGLProgram::~OpenGLProgram()
{
    glDeleteProgram(programID); $gl_chk
    spdlog::debug("destroyed OpenGLProgram {}", programID);
}

void OpenGLProgram::Use()
{
    glUseProgram(programID); $gl_chk
}

bool OpenGLProgram::CreatePipelineFromFiles(
    const char* vertPath,
    const char* fragPath,
    const char* geomPath,
    const char* tescPath,
    const char* tesePath)
{
    assert(vertPath && fs::exists(vertPath));
    assert(fragPath && fs::exists(fragPath));
    SetShaderSource(GL_VERTEX_SHADER, fs::path(vertPath));
    SetShaderSource(GL_FRAGMENT_SHADER, fs::path(fragPath));
    if (geomPath) {
        assert(fs::exists(geomPath));
        SetShaderSource(GL_GEOMETRY_SHADER, fs::path(geomPath));
    }
    if (tescPath) {
        assert(fs::exists(tescPath));
        SetShaderSource(GL_TESS_CONTROL_SHADER, fs::path(tescPath));
    }
    if (tesePath) {
        assert(fs::exists(tesePath));
        SetShaderSource(GL_TESS_EVALUATION_SHADER, fs::path(tesePath));
    }
    
    if (!CompileShaders())
        return false;
    
    for (const auto& [type, shader] : shaders) {
        if (!shader.compiled) continue;
        if (!shader.Attach(programID)) {
            return false;
        }
    }
    
    return Link();
}

bool OpenGLProgram::AttachShader(GLenum type)
{
    return shaders.at(type).Attach(programID);
}

bool OpenGLProgram::Link()
{
    glLinkProgram(programID); $gl_chk
    int status;
    glGetProgramiv(programID, GL_LINK_STATUS, &status); $gl_chk
    if (!status) {
        std::string log(2048, '\0');
        glGetProgramInfoLog(programID, log.max_size(), NULL, log.data()); $gl_chk
        spdlog::error("program linking failed:\n{}", log);
        return false;
    }
    return true;
}

void OpenGLProgram::SetShaderSource(GLenum type, const fs::path& path, bool compile)
{
    assert(fs::exists(path));
    if (!shaders.count(type)) {
        shaders.emplace(type, type);
    }
    shaders.at(type).SetSourceFromFile((const char*)path.c_str(), compile);
    shaders.at(type).label = path.filename().string();
}

bool OpenGLProgram::CompileShaders()
{
    for (const auto& [type, shader] : shaders) {
        if (!shader.source.empty() && !shader.Compile()) {
            return false;
        }
    }
    return true;
}

GLuint OpenGLProgram::GetID()
{
    return programID;
}

void OpenGLProgram::SetUniform(const char *name, int value) const
{
    GLint location;
    location = glGetUniformLocation(programID, name); $gl_chk
    if (location == -1) {
        spdlog::error("uniform {} not found", name);
    }
    glUniform1i(location, value); $gl_chk
}

void OpenGLProgram::SetUniform(const char *name, float value) const
{
    GLint location;
    location = glGetUniformLocation(programID, name); $gl_chk
    if (location == -1) {
        spdlog::error("uniform {} not found", name);
    }
    glUniform1f(location, value); $gl_chk
}

void OpenGLProgram::SetUniform(const char *name, glm::vec2 value) const
{
    GLint location;
    location = glGetUniformLocation(programID, name); $gl_chk
    if (location == -1) {
        spdlog::error("uniform {} not found", name);
    }
    glUniform2f(location, value.x, value.y); $gl_chk
}

void OpenGLProgram::SetUniform(const char *name, glm::vec3 value) const
{
    GLint location;
    location = glGetUniformLocation(programID, name); $gl_chk
    if (location == -1) {
        spdlog::error("uniform {} not found", name);
    }
    glUniform3f(location, value.x, value.y, value.z); $gl_chk
}

void OpenGLProgram::SetUniform(const char *name, glm::vec4 value) const
{
    GLint location;
    location = glGetUniformLocation(programID, name); $gl_chk
    if (location == -1) {
        spdlog::error("uniform {} not found", name);
    }
    glUniform4f(location, value.x, value.y, value.z, value.w); $gl_chk
}

void OpenGLProgram::SetUniform(const char* name, glm::mat2 value) const
{
    GLint location;
    location = glGetUniformLocation(programID, name); $gl_chk
    if (location == -1) {
        spdlog::error("uniform {} not found", name);
    }
    glUniformMatrix2fv(location, 1, GL_FALSE, &value[0][0]); $gl_chk
}

void OpenGLProgram::SetUniform(const char* name, glm::mat3 value) const
{
    GLint location;
    location = glGetUniformLocation(programID, name); $gl_chk
    if (location == -1) {
        spdlog::error("uniform {} not found", name);
    }
    glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]); $gl_chk
}

void OpenGLProgram::SetUniform(const char* name, glm::mat4 value) const
{
    GLint location;
    location = glGetUniformLocation(programID, name); $gl_chk
    if (location == -1) {
        spdlog::error("uniform {} not found", name);
    }
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]); $gl_chk
}

void OpenGLProgram::SetGLClearFlags(GLbitfield flags)
{
    clearFlags = flags;
}

void OpenGLProgram::SetClearColor(glm::vec4 color)
{
    clearColor = color;
    glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w); $gl_chk
}

glm::vec4 OpenGLProgram::GetClearColor()
{
    return clearColor;
}

GLuint OpenGLProgram::AttribLocation(const char *attributeName) const
{
    GLint loc = glGetAttribLocation(this->programID, attributeName);
    if (loc == -1) {
        spdlog::error("OpenGLProgram::AttribLocation: Attribute {} not found", attributeName);
    }
    return (GLuint)loc;
}

void OpenGLProgram::Clear()
{
    glClear(clearFlags); $gl_chk
}