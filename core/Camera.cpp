#include <Camera.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/transform.hpp>

glm::mat4 Camera::view() const
{
    return glm::euler() * glm::translate(-this->pos);
}

glm::mat4 Camera::proj() const
{
    return glm::mat4();
}
