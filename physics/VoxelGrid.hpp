#pragma once

#include <Eigen/Dense>
#include <unordered_map>
#include <memory>
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
    void sampleVoxelVelocity(Eigen::Vector3f& vertexVel,const Eigen::Vector3f& index);
    size_t getSpatialHash(const Eigen::Vector3f& pos);

    std::mutex voxelMutex;
    // Stores the density of each voxel vertex, which is based on the number of hair vertices that are in the voxel
    std::vector<float> voxelMasses;
    // Stores the average velocity around the voxel vertex
    std::vector<Eigen::Vector3f> voxelVelocities;
    // Number of voxels in each direction
    inline size_t cellExtent() const {
        return (size_t)std::ceil(voxelGridExtent/voxelSize);
    }
    // Total number of voxels
    inline size_t nVoxels() const {
        return cellExtent()*cellExtent()*cellExtent();
    }

     // Side length of cube that makes up the voxel grid
    float voxelGridExtent = 8.0f;
    // Side length of each voxel
    float voxelSize = 1.0f;
    // Center of voxel grid
    Eigen::Vector3f origin = {0.0f, 0.0f, 0.0f};
};

