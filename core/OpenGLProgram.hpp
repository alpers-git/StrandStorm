#pragma once
#include <memory>
#include <optional>
#include <Util.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <functional>
#include <cstring>
#include <string>
#include <vector>

#define $gl_chk ;_checkGLError(__FILE__, __LINE__);

struct Shader
{
    GLuint glID = GL_INVALID_INDEX;
    std::string source;
    std::string label = "(unlabeled shader)";
    mutable bool compiled = false;

    Shader(GLenum type) : glID(glCreateShader(type)) {};
    ~Shader();
    bool Compile() const;
    bool Attach(GLuint programID) const;
    void SetSource(const std::string& src, bool compile = false);
    void SetSourceFromFile(const char* filePath, bool compile = false);
};

struct TextureParams
{
    TextureParams() {};
    GLenum minFilter = GL_LINEAR;
    GLenum magFilter = GL_LINEAR;
    GLenum wrapS = GL_CLAMP_TO_EDGE;
    GLenum wrapT = GL_CLAMP_TO_EDGE;
    GLenum wrapR = GL_CLAMP_TO_EDGE;
    GLenum internalFormat = GL_RGBA;
    GLenum format = GL_RGBA;
    GLenum type = GL_UNSIGNED_BYTE;
    GLuint mipMapLevel = 0;
};

struct Texture
{
    //Texture(const char* path, GLenum texUnit = GL_TEXTURE0);
    Texture(glm::uvec2 dims, GLenum texUnit = GL_TEXTURE0, TextureParams params = TextureParams());
    ~Texture() {};

    Texture(const Texture& other);
    Texture& operator=(const Texture& other);

    void Bind();
    virtual void Delete();

    GLuint glID;
    glm::uvec2 dims;

    GLuint texUnit;
};

struct DepthTexture : public Texture
{
    DepthTexture(glm::uvec2 dims, GLenum texUnit = GL_TEXTURE5, TextureParams params = TextureParams());
    ~DepthTexture() {};

    DepthTexture(const DepthTexture& other);
    DepthTexture& operator=(const DepthTexture& other);

    void Delete() override;
    void Render(std::function <void()> renderFunc);

    GLuint frameBufferID;
};

struct ShadowTexture : public DepthTexture
{
    ShadowTexture(glm::uvec2 dims, GLenum texUnit = GL_TEXTURE5, TextureParams params = TextureParams());
    ~ShadowTexture() {};

    ShadowTexture(const ShadowTexture& other);
    ShadowTexture& operator=(const ShadowTexture& other);
};

struct RenderedTexture : public Texture
{
    RenderedTexture(glm::uvec2 dims, GLenum texUnit = GL_TEXTURE10, TextureParams params = TextureParams());
    ~RenderedTexture() {};

    RenderedTexture(const RenderedTexture& other);
    RenderedTexture& operator=(const RenderedTexture& other);

    void Delete();

    void Render(std::function <void()> renderFunc);

    GLuint frameBufferID;
    GLuint depthBufferID;
};


class OpenGLProgram
{
public:
    const std::string label;
    
    OpenGLProgram(const std::string& label = "(unlabeled program)");
    ~OpenGLProgram();

    void Use() const;

    bool CreatePipelineFromFiles(
        const char* vertPath,
        const char* fragPath,
        const char* geomPath = nullptr,
        const char* tescPath = nullptr,
        const char* tesePath = nullptr);
    
    bool AttachShader(GLenum type);
    bool Link();

    void SetShaderSource(GLenum type, const fs::path& path, bool compile = false); 
    
    bool CompileShaders();
    
    GLuint GetID();

    void SetUniform(const char* name, int value, bool required = true) const;
    void SetUniform(const char* name, float value, bool required = true) const;

    void SetUniform(const char* name, glm::vec2 value, bool required = true) const;
    void SetUniform(const char* name, glm::vec3 value, bool required = true) const;
    void SetUniform(const char* name, glm::vec4 value, bool required = true) const;

    void SetUniform(const char* name, glm::mat2 value, bool required = true) const;
    void SetUniform(const char* name, glm::mat3 value, bool required = true) const;
    void SetUniform(const char* name, glm::mat4 value, bool required = true) const;

    void SetGLClearFlags(GLbitfield flags);
    void SetClearColor(glm::vec4 color);
    glm::vec4 GetClearColor();
    
    GLuint AttribLocation(const char* attributeName) const;

    // Binds buffer and sets vertex attribute pointer
    void SetAttribPointer(GLuint bufferID, const char* attrName, GLint size, GLenum type, size_t stride=0u, size_t offset=0u) const;

    void Clear();
    
    
private:
    GLuint programID = GL_INVALID_INDEX;
    std::unordered_map<GLenum, Shader> shaders;
    GLbitfield clearFlags = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
    glm::vec4 clearColor = glm::vec4(0.02f, 0.02f, 0.02f, 1.f);
};
