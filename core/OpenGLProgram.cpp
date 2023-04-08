#include <OpenGLProgram.hpp>
#include <Logging.hpp>

Shader::Shader(GLuint shader_type, std::string source)
    :source(source)
{
    glID = glCreateShader(shader_type); $gl_chk
}

Shader::~Shader()
{
    glDeleteShader(glID); $gl_chk
}

bool Shader::Compile()
{
    const char* sourceCharArr = this->source.c_str();
    glShaderSource(glID, 1, &sourceCharArr, NULL); $gl_chk
    glCompileShader(glID); $gl_chk
    int success;
    glGetShaderiv(glID, GL_COMPILE_STATUS, &success); $gl_chk
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(glID, 512, NULL, infoLog); $gl_chk
        spdlog::error("shader compilation failed:\n{}", infoLog);
    }
    return true;
}

bool Shader::AttachShader(GLuint program)
{
    glAttachShader(program, glID); $gl_chk
    glLinkProgram(program); $gl_chk
    int status;
    glGetProgramiv(program, GL_LINK_STATUS, &status); $gl_chk
    if (!status) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog); $gl_chk
        spdlog::error("shader compilation failed:\n{}", infoLog);
    }
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
    glID = glCreateProgram(); $gl_chk
    glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w); $gl_chk
}

OpenGLProgram::~OpenGLProgram()
{
    glDeleteProgram(glID); $gl_chk
    spdlog::debug("destroyed OpenGLProgram {}", glID);
};

void OpenGLProgram::Use()
{
    glUseProgram(glID); $gl_chk
}

bool OpenGLProgram::CreatePipelineFromFiles(const char* filePathVert, const char* filePathFrag)
{
    SetVertexShaderSourceFromFile(filePathVert);
    SetFragmentShaderSourceFromFile(filePathFrag);
    
    if (!CompileShaders())
        return false;
    
    AttachVertexShader();
    AttachFragmentShader();
    return true;
}

bool OpenGLProgram::CreatePipeline()
{
    if (!CompileShaders())
        return false;

    AttachVertexShader();
    AttachFragmentShader();
    return true;
}

bool OpenGLProgram::AttachVertexShader()
{
    if (vertexShader.AttachShader(glID))
        return true;
    else
        return false;

    return true;
}

bool OpenGLProgram::AttachFragmentShader()
{
    if (fragmentShader.AttachShader(glID))
        return true;
    else
        return false;
    return true;
}

void OpenGLProgram::SetVertexShaderSource(const char *src, bool compile)
{
    vertexShader.SetSource(src, compile);
}

void OpenGLProgram::SetFragmentShaderSource(const char *src, bool compile)
{
    fragmentShader.SetSource(src, compile);
}

void OpenGLProgram::SetVertexShaderSourceFromFile(const char *filePath, bool compile)
{
    vertexShader.SetSourceFromFile(filePath, compile);
}

void OpenGLProgram::SetFragmentShaderSourceFromFile(const char *filePath, bool compile)
{
    fragmentShader.SetSourceFromFile(filePath, compile);
}

void OpenGLProgram::SetVertexShader(Shader& shader)
{
    vertexShader = std::move(shader);
}

void OpenGLProgram::SetFragmentShader(Shader& shader)
{
    fragmentShader = std::move(shader);
}

bool OpenGLProgram::CompileShaders()
{
    if (!vertexShader.Compile())
        return false;
    if (!fragmentShader.Compile())
        return false;
    return true;
};

GLuint OpenGLProgram::GetID()
{
    return glID;
}

void OpenGLProgram::SetUniform(const char *name, int value) const
{
    GLint location;
    location = glGetUniformLocation(glID, name); $gl_chk
    if (location == -1) {
        spdlog::error("uniform {} not found", name);
    }
    glUniform1i(location, value); $gl_chk
}

void OpenGLProgram::SetUniform(const char *name, float value) const
{
    GLint location;
    location = glGetUniformLocation(glID, name); $gl_chk
    if (location == -1) {
        spdlog::error("uniform {} not found", name);
    }
    glUniform1f(location, value); $gl_chk
}

void OpenGLProgram::SetUniform(const char *name, glm::vec2 value) const
{
    GLint location;
    location = glGetUniformLocation(glID, name); $gl_chk
    if (location == -1) {
        spdlog::error("uniform {} not found", name);
    }
    glUniform2f(location, value.x, value.y); $gl_chk
}

void OpenGLProgram::SetUniform(const char *name, glm::vec3 value) const
{
    GLint location;
    location = glGetUniformLocation(glID, name); $gl_chk
    if (location == -1) {
        spdlog::error("uniform {} not found", name);
    }
    glUniform3f(location, value.x, value.y, value.z); $gl_chk
}

void OpenGLProgram::SetUniform(const char *name, glm::vec4 value) const
{
    GLint location;
    location = glGetUniformLocation(glID, name); $gl_chk
    if (location == -1) {
        spdlog::error("uniform {} not found", name);
    }
    glUniform4f(location, value.x, value.y, value.z, value.w); $gl_chk
}

void OpenGLProgram::SetUniform(const char* name, glm::mat2 value) const
{
    GLint location;
    location = glGetUniformLocation(glID, name); $gl_chk
    if (location == -1) {
        spdlog::error("uniform {} not found", name);
    }
    glUniformMatrix2fv(location, 1, GL_FALSE, &value[0][0]); $gl_chk
}

void OpenGLProgram::SetUniform(const char* name, glm::mat3 value) const
{
    GLint location;
    location = glGetUniformLocation(glID, name); $gl_chk
    if (location == -1) {
        spdlog::error("uniform {} not found", name);
    }
    glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]); $gl_chk
}

void OpenGLProgram::SetUniform(const char* name, glm::mat4 value) const
{
    GLint location;
    location = glGetUniformLocation(glID, name); $gl_chk
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
    GLint loc = glGetAttribLocation(this->glID, attributeName);
    if (loc == -1) {
        spdlog::error("OpenGLProgram::AttribLocation: Attribute {} not found", attributeName);
    }
    return (GLuint)loc;
}

void OpenGLProgram::Clear()
{
    glClear(clearFlags); $gl_chk
}