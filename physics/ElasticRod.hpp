#pragma once

#include <Util.hpp>
#include <Eigen/Dense>

using namespace Eigen;

class ElasticRod
{
private:
    std::vector<Vector3f> xRest;
    std::vector<Vector3f> x;
    std::vector<Vector3f> v;

    // Curvature
    float kappa(size_t i);
    // Curvature binormal
    Vector3f kappaB(size_t i);
    // Bending energy
    float bendingEnergy();
    // Initial edge length
    float initEdgeLen(size_t i);
    // Gradient of psi i wrt x j
    Vector3f psiGrad(size_t i, size_t j);
    // Gradient of curvature binormal i wrt x j
    Matrix3f kappaBGrad(size_t i, size_t j);
    // Edge vector (i+1) - i
    Vector3f edge(size_t i);
    // Force acting on vertex i
    Vector3f force(size_t i);
public:

    // Bending modulus
    float alpha = 0.1f;
    // Something
    float beta = 0.1f;

    void init(const std::vector<glm::vec3>& verts);
    void integrateFwEuler(float dt);
    void updateVerts(std::vector<glm::vec4>& verts);
};