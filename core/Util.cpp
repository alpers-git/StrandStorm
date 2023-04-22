#include <Util.hpp>
#include <Logging.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <OpenGLProgram.hpp>

void _checkGLError(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        spdlog::error("GL_{} - {}:{}", error, fs::path(file).filename().string(), line);
    }
}

void APIENTRY GLDebugMessageCallback(
    GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar *msg, const void *data)
{
    std::string _source;
    switch (source) {
        case GL_DEBUG_SOURCE_API:               _source = "api"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     _source = "window"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:   _source = "shader"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:       _source = "3rd party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:       _source = "app"; break;
        case GL_DEBUG_SOURCE_OTHER:             _source = "UNKNOWN"; break;
        default: _source = "UNKNOWN"; break;
    }

    std::string _type;
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:               _type = "error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: _type = "deprecated"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  _type = "undefined"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         _type = "portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         _type = "performance"; break;
        case GL_DEBUG_TYPE_OTHER:               _type = "other"; break;
        case GL_DEBUG_TYPE_MARKER:              _type = "marker"; break;
        default: _type = "UNKNOWN"; break;
    }

    std::string _severity;
    spdlog::level::level_enum lvl;
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            _severity = "high";
            lvl = spdlog::level::warn;
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            _severity = "med";
            lvl = spdlog::level::warn;
            break;
        case GL_DEBUG_SEVERITY_LOW:
            _severity = "low";
            lvl = spdlog::level::debug;
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            _severity = "notif";
            lvl = spdlog::level::trace;
            break;
        default:
            _severity = "UNKNOWN";
            lvl = spdlog::level::debug;
            break;
    }

    if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
        spdlog::log(lvl, "GL[{}|{}|{}] {}", _type, _severity, _source, msg);
    }
}

/* Random Number Generator Class */
RNG::RNG(uint32_t seed) : seed(seed), gen(seed) {
}
int RNG::range(int a, int b) {
    const std::uniform_int_distribution<int>::param_type params(a, b);
    return this->idist(gen, params);
}
float RNG::range(float a, float b) {
    const std::uniform_real_distribution<float>::param_type params(a, b);
    return this->rdist(gen, params);
}
glm::vec3 RNG::vec(const glm::vec3& min, const glm::vec3& max) {
    return {this->range(min.x, max.x), this->range(min.y, max.y), this->range(min.z, max.z)};
}
glm::vec3 RNG::vec(const glm::vec3& max) {
    return this->vec({0.0f, 0.0f, 0.0f}, max);
}
glm::vec3 RNG::rotation()
{
    return {this->range(0.0f, tau), this->range(0.0f, tau), this->range(0.0f, tau)};
}
bool RNG::test(float probability) {
    const std::uniform_real_distribution<float>::param_type params(0, 1);
    return this->rdist(gen, params) < probability;
}

ImVec2 make_ImVec2(const glm::vec2 &v)
{
    return ImVec2(v.x, v.y);
}

Eigen::Matrix3f skew(const Eigen::Vector3f &v)
{
    Eigen::Matrix3f m; m <<
        0.0f, -v.z(), v.y(),
        v.z(), 0.0f, -v.x(),
        -v.y(), v.x(), 0.0f;
    return m;
}

namespace glm {
    vec3 make_vec3(const cy::Vec3f &v)
    {
        return make_vec3((const float*)&v[0]);
    }
    glm::vec2 make_vec2(const ImVec2 &v)
    {
        return glm::vec2(v.x, v.y);
    }
};

// --- GL Helpers ------------------------------------------------------------

GLuint gl::buffer(GLenum target, size_t bytes, const void *data, GLenum usage)
{
    GLuint bufferID = GL_INVALID_INDEX;
    glCreateBuffers(1, &bufferID) $gl_chk;
    glBindBuffer(target, bufferID) $gl_chk;
    glBufferData(target, bytes, data, usage) $gl_chk;
    glBindBuffer(target, GL_NONE) $gl_chk;
    return bufferID;
}
