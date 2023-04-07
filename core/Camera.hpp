#pragma once

#include <Util.hpp>

class Camera
{
public:
    glm::vec3 pos = {0.0f, 0.0f, 0.0f};
    glm::vec3 rot = {0.0f, 0.0f, 0.0f};
    float near = 0.01f;
    float far = 10000.0f;
    float fov = 1.1f;

    // Returns view matrix
    glm::mat4 view() const;
    // Returns projection matrix
    glm::mat4 proj() const;
};