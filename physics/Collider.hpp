#pragma once
#include <glm/glm.hpp>


struct CollisionInfo
{
    glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);
    float penetration = 0.0f;
};

class Collider
{
public:
    Collider(glm::vec3 center);
    /*
    * Returns true if the collider is colliding with the other collider
    */
    virtual bool IsCollidingWith(Collider& other, CollisionInfo& collision) = 0;
    /*
    * Returns the distance from center of the collider in the direction from the center to pos
    */
    virtual float GetBoundaryAt(glm::vec3 pos) = 0;

    float elasticity = 1.0f;
    float friction = 0.0f;
    glm::vec3 center;
private:
};

class SphereCollider : public Collider
{
public:
    SphereCollider(glm::vec3 center, float radius);

    bool IsCollidingWith(Collider& other, CollisionInfo& collision) override;
    float GetBoundaryAt(glm::vec3 pos) override;

    float radius;
};

class BoxCollider : public Collider
{
public:
    BoxCollider(glm::vec3 center, glm::vec3 size);

    bool IsCollidingWith(Collider& other, CollisionInfo& collision) override;
    float GetBoundaryAt(glm::vec3 pos) override;

    glm::vec3 size;
};