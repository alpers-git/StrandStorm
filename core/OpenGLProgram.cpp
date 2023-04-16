#include <OpenGLProgram.hpp>
#include <Logging.hpp>

Shader::~Shader()
{
    glDeleteShader(glID) $gl_chk;
}

bool Shader::Compile() const
{
    const char* sourceCharArr = this->source.c_str();
    glShaderSource(glID, 1, &sourceCharArr, NULL) $gl_chk;
    glCompileShader(glID) $gl_chk;
    int success;
    glGetShaderiv(glID, GL_COMPILE_STATUS, &success) $gl_chk;
    if (!success) {
        std::string log(2048, '\0');
        glGetShaderInfoLog(glID, 2048, NULL, log.data()) $gl_chk;
        spdlog::error("failed to compile {}:\n{}", label, log);
        return false;
    }
    this->compiled = success;
    return true;
}

bool Shader::Attach(GLuint programID) const
{
    glAttachShader(programID, glID) $gl_chk;
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

// --- ShadowTexture ----------------------------------------------------------

ShadowTexture::ShadowTexture(glm::uvec2 dims, GLenum texUnit)
    : dims(dims), texUnit(texUnit)
{
    glGenTextures(1, &glID) $gl_chk;
    glBindTexture(GL_TEXTURE_2D, glID) $gl_chk

    glTexImage2D(GL_TEXTURE_2D,
        0, GL_DEPTH_COMPONENT32,
         dims.x, dims.y, 
         0, GL_DEPTH_COMPONENT,
          GL_FLOAT, NULL) $gl_chk;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER) $gl_chk;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER) $gl_chk;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE) $gl_chk;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL) $gl_chk;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) $gl_chk;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) $gl_chk;
    

    //save the renderer state
    GLint origFB;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &origFB) $gl_chk;

    //configure FB
    glGenFramebuffers(1, &frameBufferID) $gl_chk;
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID) $gl_chk;
    // float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor) $gl_chk;
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, glID, 0) $gl_chk;
    glDrawBuffer(GL_NONE) $gl_chk;
    glReadBuffer(GL_NONE) $gl_chk; //? may need it later

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		//preserve render state
    glBindFramebuffer(GL_FRAMEBUFFER, origFB) $gl_chk;
}

void ShadowTexture::Bind()
{
    glActiveTexture(texUnit) $gl_chk;
    glBindTexture(GL_TEXTURE_2D, glID) $gl_chk;
}

void ShadowTexture::Delete()
{
    glDeleteTextures(1, &glID) $gl_chk;
    glDeleteFramebuffers(1, &frameBufferID) $gl_chk;
}

void ShadowTexture::Render(std::function <void()> renderFunc)
{
    //preserve render state
    GLint origFB;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &origFB) $gl_chk;
    GLint origViewport[4];
    glGetIntegerv(GL_VIEWPORT, origViewport) $gl_chk;

    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID) $gl_chk;
    glViewport(0, 0, dims.x, dims.y) $gl_chk;
    glClear(GL_DEPTH_BUFFER_BIT) $gl_chk;
    renderFunc();
    glBindFramebuffer(GL_FRAMEBUFFER, 0) $gl_chk;

    //restore render state
    glViewport(origViewport[0], origViewport[1], origViewport[2], origViewport[3]) $gl_chk;
    glBindFramebuffer(GL_FRAMEBUFFER, origFB) $gl_chk;
}

// --- OpenGLProgram ----------------------------------------------------------

OpenGLProgram::OpenGLProgram(const std::string& label) : label(label)
{
    programID = glCreateProgram() $gl_chk;
    glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w) $gl_chk;
}

OpenGLProgram::~OpenGLProgram()
{
    glDeleteProgram(programID) $gl_chk;
    spdlog::debug("destroyed OpenGLProgram {}", programID);
}

void OpenGLProgram::Use() const
{
    glUseProgram(programID) $gl_chk;
}

bool OpenGLProgram::CreatePipelineFromFiles(
    const char* vertPath,
    const char* fragPath,
    const char* geomPath,
    const char* tescPath,
    const char* tesePath)
{
    spdlog::assrt(vertPath && fs::exists(vertPath), "({}) {} does not exist", label, vertPath);
    spdlog::assrt(fragPath && fs::exists(fragPath), "({}) {} does not exist", label, fragPath);
    SetShaderSource(GL_VERTEX_SHADER, fs::path(vertPath));
    SetShaderSource(GL_FRAGMENT_SHADER, fs::path(fragPath));
    if (geomPath) {
        spdlog::assrt(fs::exists(geomPath), "({}) {} does not exist", label, geomPath);
        SetShaderSource(GL_GEOMETRY_SHADER, fs::path(geomPath));
    }
    if (tescPath) {
        spdlog::assrt(fs::exists(tescPath), "({}) {} does not exist", label, tescPath);
        SetShaderSource(GL_TESS_CONTROL_SHADER, fs::path(tescPath));
    }
    if (tesePath) {
        spdlog::assrt(fs::exists(tesePath), "({}) {} does not exist", label, tesePath);
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
    glLinkProgram(programID) $gl_chk;
    int status;
    glGetProgramiv(programID, GL_LINK_STATUS, &status) $gl_chk;
    if (!status) {
        std::string log(2048, '\0');
        glGetProgramInfoLog(programID, log.max_size(), NULL, log.data()) $gl_chk;
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
    shaders.at(type).SetSourceFromFile(path.string().c_str(), compile);
    shaders.at(type).label = path.filename().string();
}

bool OpenGLProgram::CompileShaders()
{
    for (const auto& [type, shader] : shaders) {
        assert(!shader.source.empty());
        
        bool compiled = shader.Compile();
        if (!compiled) {
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
    location = glGetUniformLocation(programID, name) $gl_chk;
    if (location == -1) {
        spdlog::error("uniform {} not found", name);
    }
    glUniform1i(location, value) $gl_chk;
}

void OpenGLProgram::SetUniform(const char *name, float value) const
{
    GLint location;
    location = glGetUniformLocation(programID, name) $gl_chk;
    if (location == -1) {
        spdlog::error("uniform {} not found", name);
    }
    glUniform1f(location, value) $gl_chk;
}

void OpenGLProgram::SetUniform(const char *name, glm::vec2 value) const
{
    GLint location;
    location = glGetUniformLocation(programID, name) $gl_chk;
    if (location == -1) {
        spdlog::error("uniform {} not found", name);
    }
    glUniform2f(location, value.x, value.y) $gl_chk;
}

void OpenGLProgram::SetUniform(const char *name, glm::vec3 value) const
{
    GLint location;
    location = glGetUniformLocation(programID, name) $gl_chk;
    if (location == -1) {
        spdlog::error("uniform {} not found", name);
    }
    glUniform3f(location, value.x, value.y, value.z) $gl_chk;
}

void OpenGLProgram::SetUniform(const char *name, glm::vec4 value) const
{
    GLint location;
    location = glGetUniformLocation(programID, name) $gl_chk;
    if (location == -1) {
        spdlog::error("uniform {} not found", name);
    }
    glUniform4f(location, value.x, value.y, value.z, value.w) $gl_chk;
}

void OpenGLProgram::SetUniform(const char* name, glm::mat2 value) const
{
    GLint location;
    location = glGetUniformLocation(programID, name) $gl_chk;
    if (location == -1) {
        spdlog::error("uniform {} not found", name);
    }
    glUniformMatrix2fv(location, 1, GL_FALSE, &value[0][0]) $gl_chk;
}

void OpenGLProgram::SetUniform(const char* name, glm::mat3 value) const
{
    GLint location;
    location = glGetUniformLocation(programID, name) $gl_chk;
    if (location == -1) {
        spdlog::error("uniform {} not found", name);
    }
    glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]) $gl_chk;
}

void OpenGLProgram::SetUniform(const char* name, glm::mat4 value) const
{
    GLint location;
    location = glGetUniformLocation(programID, name) $gl_chk;
    if (location == -1) {
        spdlog::error("uniform {} not found", name);
    }
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]) $gl_chk;
}

void OpenGLProgram::SetGLClearFlags(GLbitfield flags)
{
    clearFlags = flags;
}

void OpenGLProgram::SetClearColor(glm::vec4 color)
{
    clearColor = color;
    glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w) $gl_chk;
}

glm::vec4 OpenGLProgram::GetClearColor()
{
    return clearColor;
}

GLuint OpenGLProgram::AttribLocation(const char *attributeName) const
{
    const GLint loc = glGetAttribLocation(this->programID, attributeName) $gl_chk;
    spdlog::assrt(loc >= 0, "OpenGLProgram({}): attribute {} not found", label, attributeName);
    return (GLuint)loc;
}

void OpenGLProgram::SetAttribPointer(GLuint bufferID, const char *attrName, GLint size, GLenum type, size_t stride, size_t offset) const
{
    Use();
    glBindBuffer(GL_ARRAY_BUFFER, bufferID) $gl_chk;
    GLuint loc = AttribLocation(attrName);
    glVertexAttribPointer(loc, size, type, GL_FALSE, stride, (void*)offset) $gl_chk;
    glEnableVertexAttribArray(loc) $gl_chk;
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE) $gl_chk;
}

void OpenGLProgram::Clear()
{
    glClear(clearFlags) $gl_chk;
}