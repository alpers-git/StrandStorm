#include <OpenGLProgram.hpp>

OpenGLProgram::OpenGLProgram()
{
    GL_CALL(glID = glCreateProgram());
    GL_CALL(glEnable(GL_DEPTH_TEST));
    GL_CALL(glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w));

    //glEnable(GL_DEBUG_OUTPUT);
}

OpenGLProgram::~OpenGLProgram()
{
    GL_CALL(glDeleteProgram(glID));
    spdlog::debug("destroyed OpenGLProgram {}", glID);
};

void OpenGLProgram::Use()
{
    GL_CALL(glUseProgram(glID));
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
        GL_CALL(glDeleteShader(vertexShader.glID)); // Lets drivers know we don't need this shader objects anymore.
    else
        return false;

    return true;
}

bool OpenGLProgram::AttachFragmentShader()
{
    if (fragmentShader.AttachShader(glID))
        GL_CALL(glDeleteShader(fragmentShader.glID)); // Lets drivers know we don't need this shader objects anymore.
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
    GL_CALL(location = glGetUniformLocation(glID, name));
    if (location == -1)
    {
        std::cout << "ERROR::SHADER::UNIFORM::" << name << "::NOT_FOUND" << std::endl;
        return;
    }
    GL_CALL(glUniform1i(location, value));
}

void OpenGLProgram::SetUniform(const char *name, float value) const
{
    GLint location;
    GL_CALL(location = glGetUniformLocation(glID, name));
    if (location == -1)
    {
        std::cout << "ERROR::SHADER::UNIFORM::" << name << "::NOT_FOUND" << std::endl;
        return;
    }
    GL_CALL(glUniform1f(location, value));
}

void OpenGLProgram::SetUniform(const char *name, glm::vec2 value) const
{
    GLint location;
    GL_CALL(location = glGetUniformLocation(glID, name));
    if (location == -1)
    {
        std::cout << "ERROR::SHADER::UNIFORM::" << name << "::NOT_FOUND" << std::endl;
        return;
    }
    GL_CALL(glUniform2f(location, value.x, value.y));
}

void OpenGLProgram::SetUniform(const char *name, glm::vec3 value) const
{
    GLint location;
    GL_CALL(location = glGetUniformLocation(glID, name));
    if (location == -1)
    {
        std::cout << "ERROR::SHADER::UNIFORM::" << name << "::NOT_FOUND" << std::endl;
        return;
    }
    GL_CALL(glUniform3f(location, value.x, value.y, value.z));
}

void OpenGLProgram::SetUniform(const char *name, glm::vec4 value) const
{
    GLint location;
    GL_CALL(location = glGetUniformLocation(glID, name));
    if (location == -1)
    {
        std::cout << "ERROR::SHADER::UNIFORM::" << name << "::NOT_FOUND" << std::endl;
        return;
    }
    GL_CALL(glUniform4f(location, value.x, value.y, value.z, value.w));
}

void OpenGLProgram::SetUniform(const char* name, glm::mat2 value) const
{
    GLint location;
    GL_CALL(location = glGetUniformLocation(glID, name));
    if (location == -1)
    {
        std::cout << "ERROR::SHADER::UNIFORM::" << name << "::NOT_FOUND" << std::endl;
        return;
    }
    GL_CALL(glUniformMatrix2fv(location, 1, GL_FALSE, &value[0][0]));
}

void OpenGLProgram::SetUniform(const char* name, glm::mat3 value) const
{
    GLint location;
    GL_CALL(location = glGetUniformLocation(glID, name));
    if (location == -1)
    {
        std::cout << "ERROR::SHADER::UNIFORM::" << name << "::NOT_FOUND" << std::endl;
        return;
    }
    GL_CALL(glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]));
}

void OpenGLProgram::SetUniform(const char* name, glm::mat4 value) const
{
    GLint location;
    GL_CALL(location = glGetUniformLocation(glID, name));
    if (location == -1)
    {
        std::cout << "ERROR::SHADER::UNIFORM::" << name << "::NOT_FOUND" << std::endl;
        return;
    }
    GL_CALL(glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]));
}

void OpenGLProgram::SetGLClearFlags(GLbitfield flags)
{
    clearFlags = flags;
}

void OpenGLProgram::SetClearColor(glm::vec4 color)
{
    clearColor = color;
    GL_CALL(glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w));
}

glm::vec4 OpenGLProgram::GetClearColor()
{
    return clearColor;
}

void OpenGLProgram::Clear()
{
    GL_CALL(glClear(clearFlags));
}