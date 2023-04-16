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

//--------------------------------------------------------------------------------

Texture::Texture(glm::uvec2 dims, GLenum texUnit, TextureParams params)
    : dims(dims), texUnit(texUnit)
{
    glGenTextures(1, &glID); $gl_chk
    glBindTexture(GL_TEXTURE_2D, glID); $gl_chk

    glTexImage2D(GL_TEXTURE_2D, params.mipMapLevel, 
        params.internalFormat, 
        dims.x, dims.y, 
        0, params.format, 
        params.type, NULL); $gl_chk

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.minFilter); $gl_chk
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.magFilter); $gl_chk
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.wrapS); $gl_chk
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.wrapT); $gl_chk
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, params.wrapR); $gl_chk//?

    //create mipmaps
    glGenerateMipmap(GL_TEXTURE_2D); $gl_chk
}

Texture::Texture(const Texture& other)
    : glID(other.glID), dims(other.dims), texUnit(other.texUnit)
{
    //other.glID = 0;
}

Texture& Texture::operator=(const Texture& other)
{
    this->glID = other.glID;
    this->dims = other.dims;
    this->texUnit = other.texUnit;
    return *this;
}

void Texture::Bind()
{
    glActiveTexture(texUnit); $gl_chk
    glBindTexture(GL_TEXTURE_2D, glID); $gl_chk
}

void Texture::Delete()
{
    glDeleteTextures(1, &glID); $gl_chk
}

ShadowTexture::ShadowTexture(glm::uvec2 dims, GLenum texUnit, TextureParams params)
    : Texture(dims, texUnit, params)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE) $gl_chk;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL) $gl_chk;

    //save the renderer state
    GLint origFB;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &origFB) $gl_chk;

    //configure FB
    glGenFramebuffers(1, &frameBufferID) $gl_chk;
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID) $gl_chk;
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, glID, 0) $gl_chk;
    glDrawBuffer(GL_NONE) $gl_chk;
    glReadBuffer(GL_NONE) $gl_chk; //? may need it later

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    //preserve render state
    glBindFramebuffer(GL_FRAMEBUFFER, origFB) $gl_chk;
}

ShadowTexture::ShadowTexture(const ShadowTexture & other)
    : Texture(other), frameBufferID(other.frameBufferID)
{
}

ShadowTexture& ShadowTexture::operator=(const ShadowTexture & other)
{
    this->glID = other.glID;
    this->dims = other.dims;
    this->texUnit = other.texUnit;
    this->frameBufferID = other.frameBufferID;
    return *this;
}


void ShadowTexture::Delete()
{
    Texture::Delete();
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

RenderedTexture::RenderedTexture(glm::uvec2 dims, GLenum texUnit, TextureParams params)
    : Texture(dims, texUnit, params)
{
    //save the renderer state
    GLint origFB;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &origFB) $gl_chk;

    //configure FB
    glGenFramebuffers(1, &frameBufferID) $gl_chk;
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID) $gl_chk;
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, glID, 0) $gl_chk;
    glDrawBuffer(GL_COLOR_ATTACHMENT0) $gl_chk;
    glReadBuffer(GL_COLOR_ATTACHMENT0) $gl_chk;

    //create depth buffer
    glGenRenderbuffers(1, &depthBufferID) $gl_chk;
    glBindRenderbuffer(GL_RENDERBUFFER, depthBufferID) $gl_chk;
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, dims.x, dims.y) $gl_chk;
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
        GL_RENDERBUFFER, depthBufferID) $gl_chk;

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    //preserve render state
    glBindFramebuffer(GL_FRAMEBUFFER, origFB) $gl_chk;
}

RenderedTexture::RenderedTexture(const RenderedTexture & other)
    : Texture(other), frameBufferID(other.frameBufferID), depthBufferID(other.depthBufferID)
{
}

RenderedTexture& RenderedTexture::operator=(const RenderedTexture & other)
{
    this->glID = other.glID;
    this->dims = other.dims;
    this->texUnit = other.texUnit;
    this->frameBufferID = other.frameBufferID;
    this->depthBufferID = other.depthBufferID;
    return *this;
}

void RenderedTexture::Delete()
{
    Texture::Delete();
    glDeleteFramebuffers(1, &frameBufferID) $gl_chk;
    glDeleteRenderbuffers(1, &depthBufferID) $gl_chk;
}

void RenderedTexture::Render(std::function <void()> renderFunc)
{
    //preserve render state
    GLint origFB;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &origFB) $gl_chk;
    GLint origViewport[4];
    glGetIntegerv(GL_VIEWPORT, origViewport) $gl_chk;

    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID) $gl_chk;
    glViewport(0, 0, dims.x, dims.y) $gl_chk;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) $gl_chk;
    renderFunc();
    glBindFramebuffer(GL_FRAMEBUFFER, 0) $gl_chk;
    Bind();
    glGenerateMipmap(GL_TEXTURE_2D); $gl_chk

    //restore render state
    glViewport(origViewport[0], origViewport[1], origViewport[2], origViewport[3]) $gl_chk;
    glBindFramebuffer(GL_FRAMEBUFFER, origFB) $gl_chk;
}

//--------------------------------------------------------------------------------

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
    shaders.at(type).SetSourceFromFile(path.string().c_str(), compile);
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

