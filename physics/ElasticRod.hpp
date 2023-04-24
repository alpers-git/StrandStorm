#pragma once

#include <array>
#include <Util.hpp>
#include <Eigen/Dense>
#include <Scene.hpp>
#include <VoxelGrid.hpp>

using namespace Eigen;



class ElasticRod
{
private:
    struct BishopFrame
    {
        Vector3f u = Vector3f::Zero();
        Vector3f v = Vector3f::Zero();
        BishopFrame() = default;
        BishopFrame(const Vector3f& u, const Vector3f& v) : u(u), v(v) {}
    };

    struct MaterialFrame
    {
        Vector3f m1 = Vector3f::Zero();
        Vector3f m2 = Vector3f::Zero();
        MaterialFrame() = default;
        MaterialFrame(const Vector3f& m1, const Vector3f& m2) : m1(m1), m2(m2) {}
    };

    // Curvature binormal
    Vector3f kappaB(int i);
    // Initial edge length
    float initEdgeLen(int i);
    // Gradient holonomy term i,j
    Vector3f psiGrad(int i, int j);
    // Gradient of curvature binormal i wrt x j
    Matrix3f kappaBGrad(int i, int j);
    // Edge vector (i+1) - i
    Vector3f edge(int i);
    // Initial edge vector (i+1) - i
    Vector3f initEdge(int i);
    // Force acting on vertex i
    Vector3f force(int i);
    // Computes next u vector via parallel transport
    Vector3f parallelTransportFrame(int i, const Vector3f& u);
    // Material curvature for (i,j)
    Vector2f omega(int i, int j);
    // Gradient of material curvature
    Matrix<float, 2, 3> omegaGrad(int i, int j, int k);
    // Compute gradient holonomy from gradient holonomy terms
    Vector3f gradHolonomy(int i, int j);
    // Energy derivative dEdX for vertex i
    Vector3f dEdX(int i);
    // Energy derivative dE/dTheta for 

    // Generates the bishop frames
    void compBishopFrames();
    // Recomputes the material frames
    void compMatFrames();
    // Computes gradient holonomy terms
    void compGradHolonomyTerms();

    // Intergrated position of vertices before inextensibility constraint
    std::vector<Vector3f> xUnconstrained; 
    // Vertex offsets for inextensibility constraint
    std::vector<Vector3f> correctionVecs; 
    // Bishop (rest) frames
    std::vector<BishopFrame> bishopFrames;
    // Material (active) frames
    std::vector<MaterialFrame> M;
    // Initial twisting vector
    Vector3f u0 = {0.0f, 0.0f, 0.0f};
    // Material curvature at rest for i, j, where j:(i-1, i, i+1)
    std::vector<std::array<Vector2f, 2u>> omega0;
    // Bending angles
    std::vector<float> theta;
    // Gradient holonomy terms (i-1, i, i+1) for each vertex
    std::vector<std::array<Vector3f, 3u>> gradHolonomyTerms;
public:
    // particle positions at rest
    std::vector<Vector3f> xRest;
    // particle positions
    std::vector<Vector3f> x;
    // particle velocities
    std::vector<Vector3f> v;

    // Gravity force added to each free vertex
    static Vector3f gravity;
    // [0,1] Simple velocity reduction factor
    static float drag;
    // [0,1] Interpolation factor for enforcing inextensibility constraint
    static float inextensibility;
    // Bending modulus (resistance to bending)
    static float alpha;
    // Bending stiffness
    static float bendingStiffness;
    // Used in voxel velocity update
    static float friction;

    ElasticRod() = default;
    ElasticRod(const std::vector<glm::vec3>& verts);

    void init(const std::vector<glm::vec3>& verts);
    void integrateFwEuler(float dt);
    void handleCollisions(const std::vector<std::shared_ptr<SceneObject>>& colliders);
    void enforceConstraints(float dt,const std::vector<std::shared_ptr<SceneObject>>& colliders);
    

    
    void setVoxelContributions(const std::shared_ptr<VoxelGrid>& voxelGrid);
    void updateAllVelocitiesFromVoxels(const std::shared_ptr<VoxelGrid>& voxelGrid);

    // Reset simulation to rest state
    void reset();
};
