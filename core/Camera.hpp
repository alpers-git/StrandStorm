#pragma once

#include <Util.hpp>

class Camera
{
private:
    float panStartTheta = 0.0f;
    float panStartPhi = 0.0f;
public:
    glm::vec3 pos = {0.0f, 0.0f, 0.0f};
    glm::vec3 rot = {0.0f, 0.0f, 0.0f};
    float near = 0.01f;
    float far = 10000.0f;
    float fov = 1.1f;
    float theta = 0.0f;
    float phi = 0.0f;
    float distance = 1.0f;
    glm::vec3 target = {0.0f, 0.0f, 0.0f};

    // Returns view matrix
    glm::mat4 view() const;
    // Returns projection matrix
    glm::mat4 proj(const glm::vec2& viewport) const;
    // Called when dragging begins
    void dragStart();
    // Universal camera control
    void control(const glm::vec2& rotateDelta, const glm::vec2& dragDelta, const glm::vec2& moveDelta);
    // Zooms in and out
    void zoom(float delta);
    //rotates camera
    void pan(glm::vec2 delta);
    //strafes camera
    void strafe(glm::vec2 delta);
    //orients camera
    void orient(glm::vec2 delta);
};