#pragma once
#include <memory>
#include <Util.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <functional>
#include <cstring>
#include <string>
#include <vector>

#define $gl_chk _checkGLError(__FILE__, __LINE__);

struct Shader
{
    GLuint glID;
    std::string source;

    Shader(GLuint shader_type, std::string source = "");
    ~Shader();
    bool Compile();
    bool AttachShader(GLuint program);
    void SetSource(const std::string& src, bool compile = false);
    void SetSourceFromFile(const char* filePath, bool compile = false);
};

class OpenGLProgram
{
public:
    
    OpenGLProgram();
    ~OpenGLProgram();

    void Use();
    
    /*
    * Read compile and attach shaders to the opengl program.
    */
    bool CreatePipelineFromFiles(const char* filePathVert, const char* filePathFrag);

    /*
    * Assuming shader sources are already set this function will
    compile and attach shaders to the opengl program.
    */
    bool CreatePipeline();
    

    bool AttachVertexShader();
    bool AttachFragmentShader();

    void SetVertexShaderSource(const char* src, bool compile = false);
    void SetFragmentShaderSource(const char* src, bool compile = false);

    void SetVertexShaderSourceFromFile(const char* filePath, bool compile = false);
    void SetFragmentShaderSourceFromFile(const char* filePath, bool compile = false);

    void SetVertexShader(Shader& shader);
    void SetFragmentShader(Shader& shader);
    
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
    GLuint glID;
    Shader vertexShader = {GL_VERTEX_SHADER};
    Shader fragmentShader = {GL_FRAGMENT_SHADER};
    GLbitfield clearFlags = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
    glm::vec4 clearColor = glm::vec4(0.02f, 0.02f, 0.02f, 1.f);
};
