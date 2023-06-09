#pragma once

#include <random>
#include <algorithm>
#include <filesystem>
#include <glad/glad.h>
#include <cyVector.h>
#include <Eigen/Dense>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <imgui.h>

namespace fs = std::filesystem;

void _checkGLError(const char *file, int line);

void APIENTRY GLDebugMessageCallback(
    GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar *msg, const void *data);


constexpr float tau = 6.283185307179586f;
constexpr float tau2 = 3.141592653589793f;
constexpr float tau4 = 1.5707963267948966f;
constexpr float pi = tau2;
constexpr float pi2 = tau4;

inline glm::vec3 spherePoint(float phi, float theta) {
    return {
        std::cos(theta) * std::sin(phi),
        std::sin(theta),
        std::cos(theta) * std::cos(phi),
    };
}

// Random number generation helper class
class RNG
{
public:
    const uint32_t seed;

private:
    std::minstd_rand gen;
    std::uniform_real_distribution<float> rdist;
    std::uniform_int_distribution<int> idist;
public:
    //Default constructor, uses specified seed for number generation
    RNG(uint32_t seed);
    //Generate a random number between 0 and 1, returns if this number is less than given probability value
    bool test(float probability);
    //Random range from a to b, inclusive
    int range(int a, int b);
    //Random range from a to b, inclusive
    float range(float a, float b);
    //Random vector3
    glm::vec3 vec(const glm::vec3& min, const glm::vec3& max);
    //Random vector3 with min 0,0,0
    glm::vec3 vec(const glm::vec3& max);
    //Random euler angles
    glm::vec3 rotation();
    //Choose random from list of items
    template <class T> T choose(const std::initializer_list<T> items) {
        auto it = items.begin();
        std::advance(it, range(0, items.size() - 1));
        return *it;
    };
};

// Generate a triangular grid of points between given triangle vertices
template<size_t N>
std::array<glm::vec3, (N * (N + 1)) / 2> tessTriangleGrid(
    const std::array<glm::vec3, 3>& p)
{
    std::array<glm::vec3, (N * (N + 1)) / 2> grid;
    size_t i = 0;
    for (size_t n = 0; n < N; n++) {
        for (size_t m = 0; m < N - n; m++) {
            float u = (float)m / (float)(N - n - 1);
            float v = (float)n / (float)(N - 1);
            float w = 1.0f - u - v;
            grid[i++] = p[0] * u + p[1] * v + p[2] * w;
        }
    }
    return std::move(grid);
}

namespace glm {
    glm::vec3 make_vec3(const cy::Vec3f& v);
    glm::vec2 make_vec2(const ImVec2& v);
};
ImVec2 make_ImVec2(const glm::vec2& v);

namespace gl {
    GLuint buffer(GLenum target, size_t bytes, const void* data = nullptr, GLenum usage = GL_STATIC_DRAW);
    template<typename T>
    GLuint buffer(GLenum target, const std::vector<T>& data, GLenum usage = GL_STATIC_DRAW)
    {
        return gl::buffer(target, (size_t)(data.size() * sizeof(T)), (const void*)data.data(), usage);
    }
};

namespace Eigen
{
    Eigen::Vector3f make_vector3f(const glm::vec3& v);
}

Eigen::Matrix3f skew(const Eigen::Vector3f &v);
template<typename T> T lerp(const T& a, const T& b, float t) {
    return a + (b - a) * t;
}
