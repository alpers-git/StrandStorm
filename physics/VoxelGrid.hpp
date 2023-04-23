#pragma once

#include <Eigen/Dense>
#include <unordered_map>
#include <mutex>

// Used in spatial hashing
static constexpr size_t prime1 = 73856093;
static constexpr size_t prime2 = 19349663;
static constexpr size_t prime3 = 83492791;

class VoxelGrid
{
public:
    VoxelGrid();
    void initVoxelGrid();
    void getVoxelCoordinates(const Eigen::Vector3f& position,Eigen::Vector3f& firstVoxelCoord,Eigen::Vector3f& localPosition);
    Eigen::Vector3f sampleVoxelVelocity(Eigen::Vector3f& vertexVel,const Eigen::Vector3f& index);
    size_t getSpatialHash(Eigen::Vector3f pos);

public:
    std::shared_ptr<std::mutex> voxelMutex;
    // Stores the density of each voxel vertex, which is based on the number of hair vertices that are in the voxel
    std::unordered_map<size_t,float> voxelMasses;
    // Stores the average velocity around the voxel vertex
    std::unordered_map<size_t,Eigen::Vector3f> voxelVelocities;   

     // Side length of cube that makes up the voxel grid
    float voxelGridExtent = 4.0f;
    // Side length of each voxel
    float voxelSize = 0.5f;     
};

