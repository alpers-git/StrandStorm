#pragma once

#include <Util.hpp>
#include <Eigen/Dense>
#include <Scene.hpp>
#include <mutex>

using namespace Eigen;

// Used in spatial hashing
static constexpr size_t prime1 = 73856093;
static constexpr size_t prime2 = 19349663;
static constexpr size_t prime3 = 83492791;

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


    // Side length of cube that makes up the voxel grid
    float voxelGridExtent = 4.0f;
    // Side length of each voxel
    float voxelSize = 0.1f;

    float friction = 0.05f;

    std::shared_ptr<std::mutex> voxelMutex;
    // Stores the density of each voxel vertex, which is based on the number of hair vertices that are in the voxel
    std::unordered_map<size_t,float> voxelMasses;
    // Stores the average velocity around the voxel vertex
    std::unordered_map<size_t,Eigen::Vector3f> voxelVelocities;    
    
    void initVoxelGrid();
    void getVoxelCoordinates(const Eigen::Vector3f& position,Eigen::Vector3f& firstVoxelCoord,Eigen::Vector3f& localPosition);
    void setVoxelContributions();
    Eigen::Vector3f sampleVoxelVelocity(Eigen::Vector3f& vertexVel,const Eigen::Vector3f& index);
    void updateAllVelocitiesFromVoxels();

    size_t getSpatialHash(Eigen::Vector3f pos);

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
    void integrateFwEuler(float dt);
    void handleCollisions(const std::vector<std::shared_ptr<SceneObject>>& colliders);
    void enforceConstraints(float dt,const std::vector<std::shared_ptr<SceneObject>>& colliders);
    
    // Reset simulation to rest state
    void reset();
};