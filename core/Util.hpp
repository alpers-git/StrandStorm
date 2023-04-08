#pragma once

#include <algorithm>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

void _checkGLError(const char *file, int line);

void APIENTRY GLDebugMessageCallback(
    GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar *msg, const void *data);

constexpr float tau = 6.283185307179586f;
constexpr float tau2 = 3.141592653589793f;
constexpr float tau4 = 1.5707963267948966f;

inline glm::vec3 spherePoint(float phi, float theta) {
    return {
        std::cos(theta) * std::sin(phi),
        std::sin(theta),
        std::cos(theta) * std::cos(phi),
    };
}