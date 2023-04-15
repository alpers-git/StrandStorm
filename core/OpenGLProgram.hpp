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
    GLuint glID;
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

struct ShadowTexture
{
    ShadowTexture(glm::uvec2 dims, GLenum texUnit = GL_TEXTURE5);
    ~ShadowTexture() {};

    void Bind();
    void Delete();

    void Render(std::function <void()> renderFunc);

    GLuint frameBufferID;
	//GLuint depthBufferID;
	GLuint glID;
	glm::uvec2 dims;

	GLuint texUnit;
};


class OpenGLProgram
{
public:
    
    OpenGLProgram();
    ~OpenGLProgram();

    void Use();

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

    void SetUniform(const char* name, int value) const;
    void SetUniform(const char* name, float value) const;

    void SetUniform(const char* name, glm::vec2 value) const;
    void SetUniform(const char* name, glm::vec3 value) const;
    void SetUniform(const char* name, glm::vec4 value) const;

    void SetUniform(const char* name, glm::mat2 value) const;
    void SetUniform(const char* name, glm::mat3 value) const;
    void SetUniform(const char* name, glm::mat4 value) const;

    void SetGLClearFlags(GLbitfield flags);
    void SetClearColor(glm::vec4 color);
    glm::vec4 GetClearColor();
    
    GLuint AttribLocation(const char* attributeName) const;

    void Clear();
    
    
private:
    GLuint programID;
    std::unordered_map<GLenum, Shader> shaders;
    GLbitfield clearFlags = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
    glm::vec4 clearColor = glm::vec4(0.02f, 0.02f, 0.02f, 1.f);
};
