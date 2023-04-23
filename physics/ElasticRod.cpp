#include <ElasticRod.hpp>

// Elastic rod sim constants
float ElasticRod::drag = 75.0f;
float ElasticRod::inextensibility = 0.1f;
float ElasticRod::alpha = 0.1f;
Vector3f ElasticRod::gravity = {0.0f, -9.8f, 0.0f};

float ElasticRod::kappa(int i)
{
    assert(i > 0);
    float phi = pi - std::acos(edge(i-1).dot(edge(i)) / (edge(i-1).norm() * edge(i).norm()));
    return 2.0f * std::tan(phi / 2.0f);
}

Vector3f ElasticRod::kappaB(int i)
{
    return (2.0f * edge(i-1).cross(edge(i))) /
           (initEdge(i).norm() * initEdge(i-1).norm() + edge(i-1).dot(edge(i)));
}

float ElasticRod::bendingEnergy()
{
    float energy = 0.0f;
    for (int i = 1; i < x.size() - 1; i++)
    {
        energy += kappaB(i).squaredNorm() / initEdgeLen(i);
    }
    return alpha * energy;
}

float ElasticRod::initEdgeLen(int i)
{
    i = std::clamp(i, 0, (int)(xRest.size() - 2));
    return (xRest[i + 1] - xRest[i]).norm();
}

Vector3f ElasticRod::psiGrad(int i, int j)
{
    assert(j >= i-1 && j <= i+1);
    const Vector3f kb = kappaB(i);
    if (j == i - 1)
    {
        return kb / (2.0f * initEdgeLen(i - 1));
    }
    else if (j == i + 1)
    {
        return -kb / (2.0f * initEdgeLen(i));
    }
    return -kb / (2.0f * initEdgeLen(i - 1)) - kb / (2.0f * initEdgeLen(i));
}

Matrix3f ElasticRod::kappaBGrad(int i, int j)
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
    return -((2.0f * skew(edge(i)) + kb * edge(i).transpose()) +
        (2.0f * skew(edge(i-1)) - kb * edge(i-1).transpose())) /
        denom;
}

Vector2f ElasticRod::omega(int i, int j) {
    assert(j >= i-1 && j <= i+1);
    const Vector3f kb = kappaB(i);
    return {
        kb.dot(M[i-1].m2),
        kb.dot(M[i-1].m1)
    };
}

const Matrix2f J = Rotation2Df(pi / 2.0f).matrix();

Matrix<float, 2, 3> ElasticRod::omegaGrad(int i, int j, int k)
{
    Matrix<float, 2, 3> m;
    m.row(0) = M[j].m2;
    m.row(1) = -M[j].m1;
    return m * kappaBGrad(i, k) - J * omega(k, j) * gradHolonomy(i, j).transpose();
}

Vector3f ElasticRod::gradHolonomy(int i, int j)
{
    Vector3f gh = Vector3f::Zero();
    if (j >= i-1 && i > 1 && i-1 < x.size()) {
        gh += gradHolonomyTerms[i-1][2];
    }
    if (j >= i && i < x.size()) {
        gh += gradHolonomyTerms[i][1];
    }
    if (j >= i+1 && i+1 < x.size()) {
        gh += gradHolonomyTerms[i+1][0];
    }
    return gh;
}

void ElasticRod::genBishopFrames()
{
    bishopFrames.resize(x.size() - 1);
    bishopFrames[0] = {u0, edge(0).cross(u0).normalized()};
    for (int i = 1; i < x.size(); i++) {
        Vector3f u = parallelTransportFrame(i, bishopFrames[i-1].u);
        Vector3f v = edge(i).cross(u).normalized();
        bishopFrames[i] = {u, v};
    }
}

void ElasticRod::compMatFrames()
{
    M.resize(x.size() - 1);
    for (int i = 0; i < x.size() - 1; i++) {
        M[i] = {
            std::cos(theta[i]) * bishopFrames[i].u + std::sin(theta[i]) * bishopFrames[i].v,
            -std::sin(theta[i]) * bishopFrames[i].u + std::cos(theta[i]) * bishopFrames[i].v
        };
    }
}

void ElasticRod::compGradHolonomyTerms()
{
    for (int i = 0; i < x.size(); i++) {
        for (int j = i-1; j <= i+1; j++) {
            gradHolonomyTerms[i][j] = psiGrad(i, j);
        }
    }
}

Vector3f ElasticRod::parallelTransportFrame(int i, const Vector3f& u) {
    const Vector3f e0 = edge(i-1);
    const Vector3f e1 = edge(i);
    Vector3f axis = (2.0f * e0.cross(e1)) / (e0.norm() * e1.norm() + e0.dot(e1));
    
    float magnitude = axis.dot(axis);
    float cosPhi = std::sqrt(4.0f / (4.0f + magnitude));
    float sinPhi = std::sqrt(magnitude / (4.0f + magnitude));
    assert(cosPhi >= 0.0f && cosPhi <= 1.0f);

    if (1.0f - cosPhi < 1e-6f) {
        return e1.cross(u).cross(e1).normalized();
    }

    Quaternionf q({cosPhi, sinPhi * axis.normalized()});
    Quaternionf p({0.0f, u});
    Quaternionf r = q * p * q.conjugate();
    return {r.y(), r.z(), r.w()};
}

Vector3f ElasticRod::edge(int i)
{
    i = std::clamp(i, 0, (int)(x.size() - 2));
    return x[i + 1] - x[i];
}

Vector3f ElasticRod::initEdge(int i)
{
    i = std::clamp(i, 0, (int)(x.size() - 2));
    return xRest[i + 1] - xRest[i];
}

Vector3f ElasticRod::force(int i)
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
    gradHolonomyTerms.resize(verts.size());

    xUnconstrained.resize(verts.size());
    correctionVecs.resize(verts.size());

    for (int i = 0; i < verts.size(); i++) 
    {
        x[i] = Vector3f(verts[i][0], verts[i][1], verts[i][2]);
        v[i] = Vector3f::Zero();
        xRest[i] = x[i];
        theta[i] = 0.0f;

        xUnconstrained[i] = Vector3f::Zero();
        correctionVecs[i] = Vector3f::Zero();        
    }

    genBishopFrames();
    compMatFrames();

    omega0.resize(verts.size());
    for (int i = 0; i < verts.size(); i++) {
        for (int j = i-1; j <= i+1; j++) {
            omega0[i][j] = omega(i, j);
        }
    }
}


void ElasticRod::integrateFwEuler(float dt)
{
    Eigen::Vector3f vUnconstrained = Vector3f::Zero();
    xUnconstrained[0] = xRest[0]; // so root position is known
    for (int i = 1; i < x.size(); i++) 
    {
        vUnconstrained = (force(i) + gravity) * dt;
        vUnconstrained -= 0.5f *drag * vUnconstrained.squaredNorm() * vUnconstrained.normalized() * dt;
        xUnconstrained[i] = x[i] + vUnconstrained * dt;
    }
}

void ElasticRod::handleCollisions(const std::vector<std::shared_ptr<SceneObject>>& colliders)
{
    SphereCollider vertCollider(Eigen::Vector3f(0.0f, 0.0f, 0.0f),1.0f);
    CollisionInfo collisionInfo;
    for (int i = 1; i < x.size(); i++) 
    {   
        for (const std::shared_ptr<SceneObject>& c : colliders)
        {
            vertCollider.center = xUnconstrained[i];
            ///NOTE: Is this supposed to be used?
            Eigen::Vector3f fromCollider = xUnconstrained[i] - c->collider->center;
            if(c->collider->IsCollidingWith(vertCollider, collisionInfo))     
                xUnconstrained[i] = c->collider->center - 1.01 * collisionInfo.normal * c->collider->GetBoundaryAt(xUnconstrained[i]);
        }      
    }
}

void ElasticRod::enforceConstraints(float dt,const std::vector<std::shared_ptr<SceneObject>>& colliders)
{
    handleCollisions(colliders);

    for (int i = 1; i < x.size(); i++) 
    {    
        Eigen::Vector3f correctedX = xUnconstrained[i] - xUnconstrained[i-1];
        correctedX = xUnconstrained[i-1] + correctedX.normalized() * initEdgeLen(i-1); 
        v[i] = (correctedX - x[i]) / dt;
        x[i] = correctedX;
        correctionVecs[i] = correctedX - xUnconstrained[i];
        // xUnconstrained[i] = correctedX;
    }
    for (int i = 1; i < x.size()-1; i++)
        v[i] -= -inextensibility * correctionVecs[i+1]/dt;
}

void ElasticRod::reset()
{
    for (int i = 0; i < x.size(); i++)
    {
        x[i] = xRest[i];
        v[i] = Vector3f::Zero();
    }
}

void ElasticRod::bendingStiffness(float value)
{
    this->B = Matrix2f::Identity() * value;
}
