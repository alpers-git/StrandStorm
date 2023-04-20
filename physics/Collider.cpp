#include <Collider.hpp>
#include <glm/gtx/component_wise.hpp>

Collider::Collider(glm::vec3 center) 
    : center(center) {}

SphereCollider::SphereCollider(glm::vec3 center, float radius)
    : Collider(center), radius(radius) {}

float SphereCollider::GetBoundaryAt(glm::vec3 pos) {
    return radius;
}

bool SphereCollider::IsCollidingWith(Collider& other, CollisionInfo& collision) {
    //apply minkowski difference
    glm::vec3 diff = center - other.center;
    float dist = glm::length(diff);
    float boundary = GetBoundaryAt(diff) + other.GetBoundaryAt(-diff);

    collision.normal = diff / dist;
    collision.penetration = dist - boundary;

    return dist < boundary;
}

BoxCollider::BoxCollider(glm::vec3 center, glm::vec3 size)
    : Collider(center), size(size) {}

float BoxCollider::GetBoundaryAt(glm::vec3 pos) {
    glm::vec3 direction = pos - center; // calculate direction from center to pos
    glm::vec3 halfSize = size / 2.0f;   // calculate half of the box size in each dimension

    // calculate the distances to the boundary planes in each dimension
    glm::vec3 d = halfSize / glm::abs(direction);

    //project glm::compMin(d) onto the direction vector
    float distance = glm::dot(direction, glm::normalize(d));

    return distance;
}

bool BoxCollider::IsCollidingWith(Collider& other, CollisionInfo& collision) {
    //apply minkowski difference
    glm::vec3 diff = center - other.center;
    float dist = glm::length(diff);
    float boundary = GetBoundaryAt(diff) + other.GetBoundaryAt(-diff);

    collision.normal = diff / dist;
    collision.penetration = dist - boundary;

    return dist < boundary;
}