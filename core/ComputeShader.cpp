#include <iostream>
#include <fstream>
#include <sstream>
#include <OpenGLProgram.hpp>
#include <ComputeShader.hpp>
#include <Logging.hpp>

void checkShaderCompileErr(GLuint shaderID, const std::string& path) {
    GLint success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success) $gl_chk;
    if (!success) {
        char* infoLog = new char[2048];
        glGetShaderInfoLog(shaderID, 2048, nullptr, infoLog) $gl_chk;
        spdlog::error("compilation of {} failed:\n{}", path, infoLog);
        delete [] infoLog;
    }
}

void checkProgramLinkErr(GLuint programID, const std::string& path) {
    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success) $gl_chk;
    if (!success) {
        char* infoLog = new char[2048];
        glGetProgramInfoLog(programID, 2048, nullptr, infoLog) $gl_chk;
        spdlog::error("linking of {} failed:\n{}", path, infoLog);
        delete [] infoLog;
    }
}

void ComputeShader::compile(const std::string &path) {
    assert(fs::exists(path) && "ComputeShader::compile: path does not exist");

    // Read shader source
    std::ifstream stream(path);
    std::stringstream buffer;
    buffer << stream.rdbuf();
    stream.close();
    std::string source = buffer.str();

    // Compile shader
    const char *sourcePtr = source.c_str();
    this->shaderID = glCreateShader(GL_COMPUTE_SHADER) $gl_chk;
    glShaderSource(this->shaderID, 1, &sourcePtr, nullptr) $gl_chk;
    glCompileShader(this->shaderID) $gl_chk;
    checkShaderCompileErr(this->shaderID, path);

    this->programID = glCreateProgram() $gl_chk;
    glAttachShader(this->programID, this->shaderID) $gl_chk;
    glLinkProgram(this->programID) $gl_chk;
    checkProgramLinkErr(this->programID, "compute shader");
}

GLuint ComputeShader::createBuffer(GLuint bindingIdx, size_t bytes, GLenum target) {
    glUseProgram(this->programID) $gl_chk;
    GLuint bufferID;
    glCreateBuffers(1, &bufferID) $gl_chk;
    glBindBuffer(target, bufferID) $gl_chk;
    glBufferData(target, bytes, nullptr, GL_DYNAMIC_DRAW) $gl_chk;
    glBindBufferBase(target, bindingIdx, bufferID) $gl_chk;
    glBindBuffer(target, 0) $gl_chk;
    this->bufBindIdxMap[bindingIdx] = {.glID = bufferID, .target = target};
    return bufferID;
}

void ComputeShader::assocBuffer(GLuint bindingIdx, GLuint bufferID, GLenum target)
{
    glUseProgram(this->programID) $gl_chk;
    glBindBufferBase(target, bindingIdx, bufferID) $gl_chk;
    this->bufBindIdxMap[bindingIdx] = {.glID = bufferID, .target = target};
}

void ComputeShader::setBufferData(GLuint bindingIdx, const void *data, size_t offset, size_t bytes)
{
    spdlog::assrt(this->bufBindIdxMap.count(bindingIdx),
        "ComputeShader::setBufferData: bindingIdx not found");
    const BufferObject& buf = this->bufBindIdxMap[bindingIdx];

    glBindBuffer(buf.target, buf.glID) $gl_chk;
    glBufferSubData(buf.target, offset, bytes, data) $gl_chk;
    glBindBuffer(buf.target, 0) $gl_chk;
}

void ComputeShader::zeroBufferData(GLuint bindingIdx, size_t offset, size_t bytes)
{
    spdlog::assrt(this->bufBindIdxMap.count(bindingIdx),
        "ComputeShader::zeroBufferData: bindingIdx not found");
    const BufferObject& buf = this->bufBindIdxMap[bindingIdx];

    glBindBuffer(buf.target, buf.glID) $gl_chk;
    const std::vector<uint8_t> zeros(bytes, 0);
    glBufferSubData(buf.target, offset, bytes, zeros.data()) $gl_chk;
    glBindBuffer(buf.target, 0) $gl_chk;
}

void ComputeShader::bindBuffers()
{
    glUseProgram(this->programID) $gl_chk;
    for (const auto& [bindingIdx, buf] : this->bufBindIdxMap) {
        glBindBufferBase(buf.target, bindingIdx, buf.glID) $gl_chk;
    }
}

void ComputeShader::run(const glm::uvec3& groups)
{
    glUseProgram(this->programID) $gl_chk;
    glDispatchCompute(groups.x, groups.y, groups.z) $gl_chk;
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT) $gl_chk;
}

GLuint ComputeShader::bufId(GLuint bindingIdx)
{
    spdlog::assrt(this->bufBindIdxMap.count(bindingIdx),
        "ComputeShader::bufId: bindingIdx not found");

    return this->bufBindIdxMap[bindingIdx].glID;
}

void ComputeShader::setUniform(const char *name, GLuint value)
{
    glUseProgram(this->programID) $gl_chk;
    glUniform1ui(glGetUniformLocation(this->programID, name), value) $gl_chk;
}
