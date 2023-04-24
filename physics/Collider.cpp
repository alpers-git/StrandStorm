#include <Collider.hpp>

Collider::Collider(Eigen::Vector3f center) 
    : center(center) {}

SphereCollider::SphereCollider(Eigen::Vector3f center, float radius)
    : Collider(center), radius(radius) {}

float SphereCollider::GetBoundaryAt(Eigen::Vector3f pos) {
    return radius;
}

bool SphereCollider::IsCollidingWith(Collider& other, CollisionInfo& collision) {
    //apply minkowski difference
    Eigen::Vector3f diff = center - other.center;
    float dist = diff.norm();
    float boundary = GetBoundaryAt(diff) + other.GetBoundaryAt(-diff);

    collision.normal = diff / dist;
    collision.penetration = dist - boundary;

    return dist < radius;
}

BoxCollider::BoxCollider(Eigen::Vector3f center, Eigen::Vector3f size)
    : Collider(center), size(size) {}

float BoxCollider::GetBoundaryAt(Eigen::Vector3f pos) {
    Eigen::Vector3f direction = pos - center; // calculate direction from center to pos
    Eigen::Vector3f halfSize = size / 2.0f;   // calculate half of the box size in each dimension

    // calculate the distances to the boundary planes in each dimension
    Eigen::Vector3f d = halfSize.array() / direction.cwiseAbs().array();

    //project glm::compMin(d) onto the direction vector
    float distance = direction.dot(d.normalized());

    return distance;
}

bool BoxCollider::IsCollidingWith(Collider& other, CollisionInfo& collision) {
    //apply minkowski difference
    Eigen::Vector3f diff = center - other.center;
    float dist = diff.norm();
    float boundary = GetBoundaryAt(diff) + other.GetBoundaryAt(-diff);

    collision.normal = diff / dist;
    collision.penetration = dist - boundary;

    return dist < boundary;
}