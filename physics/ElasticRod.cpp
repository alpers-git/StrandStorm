#include <ElasticRod.hpp>

// Elastic rod sim constants
float ElasticRod::drag = 75.0f;
float ElasticRod::inextensibility = 0.1f;
float ElasticRod::alpha = 0.1f;
Vector3f ElasticRod::gravity = {0.0f, -9.8f, 0.0f};

float ElasticRod::kappa(size_t i)
{
    assert(i > 0);
    float phi = pi - std::acos(edge(i-1).dot(edge(i)) / (edge(i-1).norm() * edge(i).norm()));
    return 2.0f * std::tan(phi / 2.0f);
}

Vector3f ElasticRod::kappaB(size_t i)
{
    return (2.0f * edge(i-1).cross(edge(i))) /
           (initEdge(i).norm() * initEdge(i-1).norm() + edge(i-1).dot(edge(i)));
}

float ElasticRod::bendingEnergy()
{
    float energy = 0.0f;
    for (size_t i = 1; i < x.size() - 1; i++)
    {
        energy += kappaB(i).squaredNorm() / initEdgeLen(i);
    }
    return alpha * energy;
}

float ElasticRod::initEdgeLen(size_t i)
{
    assert(i >= 0);
    i = std::min(i, xRest.size() - 2);
    return (xRest[i + 1] - xRest[i]).norm();
}

Vector3f ElasticRod::psiGrad(size_t i, size_t j)
{
    assert(j >= i-1 && j <= i+1);
    Vector3f kb = kappaB(i);
    if (j == i - 1)
    {
        return kb / (2.0f * initEdgeLen(i - 1));
    }
    else if (j == i + 1)
    {
        return -kb / (2.0f * initEdgeLen(i));
    }
    else if (j == i)
    {
        return -kb / (2.0f * initEdgeLen(i - 1)) - kb / (2.0f * initEdgeLen(i));
    }
}

Matrix3f ElasticRod::kappaBGrad(size_t i, size_t j)
{
    assert(j >= i-1 && j <= i+1);
    const Vector3f kb = kappaB(i);
    const float denom = initEdge(i-1).norm() * initEdge(i).norm() + edge(i-1).dot(edge(i));
    if (j == i - 1) {
        return (2.0f * skew(edge(i)) + kb * edge(i).transpose()) / denom;
    }
    else if (j == i + 1) {
        return (2.0f * skew(edge(i-1)) - kb * edge(i-1).transpose()) / denom;
    }
    else if (j == i) {
        return -((2.0f * skew(edge(i)) + kb * edge(i).transpose()) +
                (2.0f * skew(edge(i-1)) - kb * edge(i-1).transpose())) /
               denom;
    }
}

Vector3f ElasticRod::edge(size_t i)
{
    // If x_i is last vertex, return e_x-1
    assert(i >= 0);
    i = std::min(i, x.size() - 2);
    return x[i + 1] - x[i];
}

Vector3f ElasticRod::initEdge(size_t i)
{
    assert(i >= 0);
    i = std::min(i, xRest.size() - 2);
    return xRest[i + 1] - xRest[i];
}

Vector3f ElasticRod::force(size_t i)
{
    assert(i >= 1);
    Vector3f f = Vector3f::Zero();
    for (int j = i - 1; j <= i + 1; j++)
    {
        // Only bending force, we need twisting from psi
        f += (-(2.0f * alpha) / initEdgeLen(j)) * kappaBGrad(i, j).transpose() * kappaB(j);
    }
    return f;
}

void ElasticRod::init(const std::vector<glm::vec3> &verts)
{
    x.resize(verts.size());
    v.resize(verts.size());
    xRest.resize(verts.size());

    xUnconstrained.resize(verts.size());
    correctionVecs.resize(verts.size());

    for (size_t i = 0; i < verts.size(); i++) 
    {
        x[i] = Vector3f(verts[i][0], verts[i][1], verts[i][2]);
        v[i] = Vector3f::Zero();
        xRest[i] = x[i];

        xUnconstrained[i] = Vector3f::Zero();
        correctionVecs[i] = Vector3f::Zero();        
    }
}


void ElasticRod::integrateFwEuler(float dt)
{
    Eigen::Vector3f vUnconstrained = Vector3f::Zero();
    xUnconstrained[0] = xRest[0]; // so root position is known
    for (size_t i = 1; i < x.size(); i++) 
    {
        vUnconstrained = (force(i) + gravity) * dt;
        vUnconstrained -= 0.5f *drag * vUnconstrained.squaredNorm() * vUnconstrained.normalized() * dt;
        xUnconstrained[i] = x[i] + vUnconstrained * dt;
    }
}

void ElasticRod::handleCollisions(const std::vector<SceneObject>& colliders)
{
    for (size_t i = 1; i < x.size(); i++) 
    {   
        Eigen::Vector3f colliderCenter = Eigen::Vector3f(colliders[0].position.x,
                                                                colliders[0].position.y,
                                                                colliders[0].position.z);
        colliderCenter = Eigen::Vector3f(0,0,0);
        
        Eigen::Vector3f fromCollider = xUnconstrained[i] - colliderCenter;
        float dist = fromCollider.norm();
        float radius = 1.0f;
        if(dist<radius)        
            xUnconstrained[i] = colliderCenter + fromCollider.normalized()*radius*1.01;
    }
}

void ElasticRod::enforceConstraints(float dt,const std::vector<SceneObject>& colliders)
{
    handleCollisions(colliders);

    for (size_t i = 1; i < x.size(); i++) 
    {    
        Eigen::Vector3f correctedX = xUnconstrained[i] - xUnconstrained[i-1];
        correctedX = xUnconstrained[i-1] + correctedX.normalized() * initEdgeLen(i-1); 
        v[i] = (correctedX - x[i]) / dt;
        x[i] = correctedX;
        correctionVecs[i] = correctedX - xUnconstrained[i];
        // xUnconstrained[i] = correctedX;
    }
    for (size_t i = 1; i < x.size(); i++)
        v[i] -= (i+1)==x.size() ? Eigen::Vector3f(0,0,0) : -inextensibility * correctionVecs[i+1]/ dt;
}

void ElasticRod::reset()
{
    for (size_t i = 0; i < x.size(); i++)
    {
        x[i] = xRest[i];
        v[i] = Vector3f::Zero();
    }
}