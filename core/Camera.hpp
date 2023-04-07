#pragma once

#include <glm/glm.hpp>

class Camera
{
private:
    glm::vec3 pos;
    glm::vec3 rot;
    float near = 0.01f;
    float far = 10000.0f;
    float fov = 1.1f;

public:
    // Returns view matrix
    glm::mat4 view() const;
    // Returns projection matrix
    glm::mat4 proj() const;
};