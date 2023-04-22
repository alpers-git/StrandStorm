#pragma once

#include <Util.hpp>
#include <Eigen/Dense>
#include <Scene.hpp>

using namespace Eigen;

class ElasticRod
{
private:

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
    // Initial edge vector (i+1) - i
    Vector3f initEdge(size_t i);
    // Force acting on vertex i
    Vector3f force(size_t i);

    // Intergrated position of vertices before inextensibility constraint
    std::vector<Vector3f> xUnconstrained; 
    // Vertex offsets for inextensibility constraint
    std::vector<Vector3f> correctionVecs; 

public:
    // particle positions at rest
    std::vector<Vector3f> xRest;
    // particle positions
    std::vector<Vector3f> x;
    // particle velocities
    std::vector<Vector3f> v;
    
    // Scene reference for collider access
    std::shared_ptr<Scene> scene;

    // Gravity force added to each free vertex
    static Vector3f gravity;
    // [0,1] Simple velocity reduction factor
    static float drag;
    // [0,1] Interpolation factor for enforcing inextensibility constraint
    static float inextensibility;
    // Bending modulus (resistance to bending)
    static float alpha;

    void init(const std::vector<glm::vec3>& verts);
    void setScene(std::shared_ptr<Scene> scene);
    void integrateFwEuler(float dt);
    void enforceConstraints(float dt);
    // Reset simulation to rest state
    void reset();
};