#include <Camera.hpp>

glm::mat4 Camera::view() const
{
    return glm::eulerAngleXYZ(this->rot.x, this->rot.y, this->rot.z) * glm::translate(-this->pos);
}

glm::mat4 Camera::proj(const glm::vec2& viewport) const
{
    return glm::perspective(this->fov, viewport.x / viewport.y, this->near, this->far);
}

void Camera::dragStart()
{
    this->panStartTheta = this->theta;
    this->panStartPhi = this->phi;
}

void Camera::control(const glm::vec2 &rotateDelta, const glm::vec2 &dragDelta, const glm::vec2 &moveDelta)
{
    this->theta = std::clamp(this->panStartTheta + dragDelta.y, -tau4, tau4);
    this->phi = this->panStartPhi + dragDelta.x;
    this->pos = this->target + spherePoint(this->phi, this->theta) * distance;
    this->rot = {this->theta, -this->phi, 0.0f};
}

void Camera::zoom(float delta)
{
    // Adjust the distance between the camera position and the target point
    distance = glm::max(distance - delta, 0.1f);
    // Update the camera position based on the new distance
    pos.x = target.x + distance * glm::sin(theta) * glm::cos(phi);
    pos.y = target.y + distance * glm::sin(phi);
    pos.z = target.z + distance * glm::cos(theta) * glm::cos(phi);
}

void Camera::pan(glm::vec2 delta)
{
    // Calculate the change in angle based on the mouse movement
    float dTheta = delta.x / 100.0f;
    float dPhi = delta.y / 100.0f;

    // Update the camera orientation
    theta += dTheta;
    phi += dPhi;

    // Keep the phi angle within the range of [-pi/2, pi/2] to prevent flipping
    phi = glm::clamp(phi, -glm::pi<float>() / 2.0f, glm::pi<float>() / 2.0f);

    //update the orientation of the camera
    rot = {phi, theta, 0.0f};

    // Update the camera position based on the new orientation
}

void Camera::strafe(glm::vec2 delta)
{
    // Calculate the camera's right and up vectors based on its current orientation
    glm::vec3 forward = glm::normalize(target - pos);
    glm::vec3 right = glm::normalize(glm::cross(forward, {0.0f, 1.0f, 0.0f}));
    glm::vec3 up = glm::normalize(glm::cross(right, forward));

    // Calculate the amount to move based on the mouse movement
    glm::vec3 dPos = -delta.x * right + delta.y * up;

    // Update the camera position and target point
    pos += dPos;
    target += dPos;
}

void Camera::orient(glm::vec2 delta)
{
    // Calculate the change in angle based on the mouse movement
    float dTheta = delta.x / 100.0f;
    float dPhi = delta.y / 100.0f;

    // Update the camera orientation
    theta += dTheta;
    phi += dPhi;

    // Keep the phi angle within the range of [-pi/2, pi/2] to prevent flipping
    phi = glm::clamp(phi, -glm::pi<float>() / 2.0f, glm::pi<float>() / 2.0f);

    // Calculate the new camera position based on the spherical coordinates
    pos.x = target.x + distance * glm::sin(theta) * glm::cos(phi);
    pos.y = target.y + distance * glm::sin(phi);
    pos.z = target.z + distance * glm::cos(theta) * glm::cos(phi);

    // Update the camera orientation
    rot = {phi, theta, 0.0f};
}