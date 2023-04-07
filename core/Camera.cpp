#include <Camera.hpp>

glm::mat4 Camera::view() const
{
    return glm::eulerAngleXYZ(this->rot.x, this->rot.y, this->rot.z) * glm::translate(-this->pos);
}

glm::mat4 Camera::proj() const
{
    return glm::mat4();
}
