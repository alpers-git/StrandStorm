#pragma once

#include <Util.hpp>

struct BufferObject
{
    GLuint glID = GL_INVALID_INDEX;
    GLenum target;
};

class ComputeShader
{
private:
    GLuint shaderID = GL_INVALID_INDEX;
    GLuint programID = GL_INVALID_INDEX;
    // Maps binding indices to buffer IDs
    std::unordered_map<GLuint, BufferObject> bufBindIdxMap;
public:
    // Compiles compute shader from file at given path
    void compile(const std::string& path);
    // Creates a new buffer object and associates it with the given binding index
    GLuint createBuffer(GLuint bindingIdx, size_t bytes, GLenum target = GL_SHADER_STORAGE_BUFFER);
    // Creates a new SSBO and associates it with the given named binding
    GLuint createBuffer(const char* name, size_t bytes, GLenum target = GL_SHADER_STORAGE_BUFFER);
    // Associates an existing buffer object with the given binding index
    void assocBuffer(GLuint bindingIdx, GLuint bufferID, GLenum target = GL_SHADER_STORAGE_BUFFER);
    // Associates an existing buffer object with given SSBO binding name
    void assocBuffer(const char* name, GLuint bufferID, GLenum target = GL_SHADER_STORAGE_BUFFER);
    // Binds the buffer associated with the given binding index to given target
    void bindAs(GLuint bindingIdx, GLenum target);
    // Sets the data of the buffer associated with the given binding index
    void setBufferData(GLuint bindingIdx, const void* data, size_t offset, size_t bytes);
    // Sets all data of the buffer associated with the given binding index to zero
    void zeroBufferData(GLuint bindingIdx, size_t offset, size_t bytes);
    // Binds all associated buffers
    void bindBuffers();
    // Runs compute shader, blocking until complete
    void run(const glm::uvec3& groups = {1, 1, 1});
    // Returns buffer id from binding index
    GLuint bufId(GLuint bindingIdx);
    // Returns the binding index of the buffer with the given name
    GLuint bufBindingIdx(const char* name, GLenum target = GL_SHADER_STORAGE_BUFFER);
    // Sets a uniform value
    void setUniform(const char* name, GLuint value);
    // Reads a single value from the buffer associated with the given binding index
    template<typename T> T readBufferData(GLuint bindingIdx, size_t offset = 0u, GLenum target = GL_SHADER_STORAGE_BUFFER) {
        glUseProgram(this->programID) $gl_chk;
        GLuint bufferID = this->bufBindIdxMap[bindingIdx];
        glBindBuffer(target, bufferID) $gl_chk;
        T data;
        glGetBufferSubData(target, offset, sizeof(T), &data) $gl_chk;
        glBindBuffer(target, 0) $gl_chk;
        return data;
    }
    // Reads an array of values from the buffer associated with the given binding index
    template<typename T> std::vector<T> readBufferDataArray(GLuint bindingIdx, size_t count, size_t offset = 0u, GLenum target = GL_SHADER_STORAGE_BUFFER) {
        glUseProgram(this->programID) $gl_chk;
        GLuint bufferID = this->bufBindIdxMap[bindingIdx];
        glBindBuffer(target, bufferID) $gl_chk;
        std::vector<T> data(count);
        glGetBufferSubData(target, offset, sizeof(T) * count, data.data()) $gl_chk;
        glBindBuffer(target, 0) $gl_chk;
        return data;
    }
};