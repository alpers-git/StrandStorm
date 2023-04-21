#include <ElasticRod.hpp>

float ElasticRod::kappa(size_t i)
{
    assert(i < x.size() - 1 && i > 0);
    Vector3f e1 = edge(i);
    Vector3f e0 = edge(i - 1);
    float phi = pi - std::acos(e0.dot(e1) / (e0.norm() * e1.norm()));
    return 2.0f * std::tan(phi / 2.0f);
}

Vector3f ElasticRod::kappaB(size_t i)
{
    assert(i < x.size() - 1 && i > 0);
    Vector3f eCurr = edge(i);
    Vector3f ePrev = edge(i - 1);
    Vector3f eRestCurr = xRest[i + 1] - xRest[i];
    Vector3f eRestPrev = xRest[i] - xRest[i - 1];

    return (2.0f * ePrev.cross(eCurr)) /
           (eRestCurr.norm() * eRestPrev.norm() + ePrev.dot(eCurr));
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
    assert(i < xRest.size() - 1);
    return (xRest[i + 1] - xRest[i]).norm();
}

Vector3f ElasticRod::psiGrad(size_t i, size_t j)
{
    assert(j >= i - 1 && j <= i + 1);
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
    assert(j >= i - 1 && j <= i + 1);
    Vector3f kb = kappaB(i);
    Vector3f eRestCurr = xRest[i + 1] - xRest[i];
    Vector3f eRestPrev = xRest[i] - xRest[i - 1];
    float denom = eRestPrev.norm() * eRestCurr.norm() + edge(i - 1).dot(edge(i));
    if (j == i - 1)
    {
        return (2.0f * skew(edge(i)) + kb * edge(i).transpose()) / denom;
    }
    else if (j == i + 1)
    {
        return (2.0f * skew(edge(i - 1)) - kb * edge(i - 1).transpose()) / denom;
    }
    else if (j == i)
    {
        return -((2.0f * skew(edge(i)) + kb * edge(i).transpose()) +
                (2.0f * skew(edge(i - 1)) - kb * edge(i - 1).transpose())) /
               denom;
    }
}

Vector3f ElasticRod::edge(size_t i)
{
    assert(i < x.size() - 1);
    return x[i + 1] - x[i];
}

Vector3f ElasticRod::force(size_t i)
{
    assert(i >= 1 && i < x.size() - 1);
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
    for (size_t i = 0; i < verts.size(); i++)
    {
        x[i] = Vector3f(verts[i][0], verts[i][1], verts[i][2]);
        v[i] = Vector3f::Zero();
        xRest[i] = x[i];
    }
}

void ElasticRod::integrateFwEuler(float dt)
{
    for (size_t i = 2; i < x.size() - 2; i++)
    {
        v[i] += (force(i) + gravity
                - drag * v[i].squaredNorm() * v[i]) * dt;
        x[i] += v[i] * dt;
    }
}

void ElasticRod::updateVerts(std::vector<glm::vec4> &verts)
{
    for (size_t i = 0; i < x.size(); i++)
    {
        verts[i] = glm::vec4(x[i][0], x[i][1], x[i][2], 1.0f);
    }
}
