#pragma once
#include <Eigen/Dense>


struct CollisionInfo
{
    Eigen::Vector3f normal = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    float penetration = 0.0f;
};

class Collider
{
public:
    Collider(Eigen::Vector3f center);
    /*
    * Returns true if the collider is colliding with the other collider
    */
    virtual bool IsCollidingWith(Collider& other, CollisionInfo& collision) = 0;
    /*
    * Returns the distance from center of the collider in the direction from the center to pos
    */
    virtual float GetBoundaryAt(Eigen::Vector3f pos) = 0;

    float elasticity = 1.0f;
    float friction = 0.0f;
    Eigen::Vector3f center;
private:
};

class SphereCollider : public Collider
{
public:
    SphereCollider(Eigen::Vector3f center, float radius);

    bool IsCollidingWith(Collider& other, CollisionInfo& collision) override;
    float GetBoundaryAt(Eigen::Vector3f pos) override;

    float radius;
};

class BoxCollider : public Collider
{
public:
    BoxCollider(Eigen::Vector3f center, Eigen::Vector3f size);

    bool IsCollidingWith(Collider& other, CollisionInfo& collision) override;
    float GetBoundaryAt(Eigen::Vector3f pos) override;

    Eigen::Vector3f size;
};