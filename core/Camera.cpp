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
