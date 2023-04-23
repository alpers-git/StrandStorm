#include <VoxelGrid.hpp>

VoxelGrid::VoxelGrid()
{
    voxelMutex = std::make_shared<std::mutex>();
}

void VoxelGrid::initVoxelGrid()
{
    int numSteps = (int)(voxelGridExtent/voxelSize);
    size_t hash;
    bool isFirstRun = voxelMasses.empty();
    for(int i = 0; i < numSteps; i++)    
        for(int j = 0; j < numSteps; j++)        
            for(int k = 0; k < numSteps; k++)
            {
                if(isFirstRun)
                {                    
                    voxelMasses.emplace_back(0);
                    voxelVelocities.emplace_back(Eigen::Vector3f::Zero());
                }
                else
                {
                    voxelMasses[i*numSteps*numSteps + j*numSteps + k] = 0;
                    voxelVelocities[i*numSteps*numSteps + j*numSteps + k] = Eigen::Vector3f::Zero();
                }               
                    
            } 
}

void VoxelGrid::getVoxelCoordinates(const Eigen::Vector3f &position, Eigen::Vector3f &firstVoxelCoord, Eigen::Vector3f &localPosition)
{
    // Use side length of cube that contains the voxel grid to determine the origin
    Eigen::Vector3f origin = {-voxelGridExtent*0.5f, -voxelGridExtent*0.5f, -voxelGridExtent*0.5f};
    Eigen::Vector3f coordsInVoxel = (position-origin)/voxelSize;
    // Get indices of voxel containing sampling point
    Eigen::Vector3f indices = Eigen::Vector3f(coordsInVoxel.array().floor());
    firstVoxelCoord = indices;
    localPosition = coordsInVoxel - indices;
}

Eigen::Vector3f VoxelGrid::sampleVoxelVelocity(Eigen::Vector3f &vertexVel, const Eigen::Vector3f &index)
{
    size_t hash = getSpatialHash(index);
    float norm = voxelMasses[hash] == 0 ? 1 : voxelMasses[hash];
    return voxelVelocities[hash]/norm;
}

// Based on the paper "Real-time 3D Reconstruction at Scale using Voxel Hashing"
size_t VoxelGrid::getSpatialHash(Eigen::Vector3f pos)
{
    int numSteps = (int)(voxelGridExtent/voxelSize);
    return pos[0]*numSteps*numSteps + pos[1]*numSteps + pos[2];
    // size_t n = (size_t)pow((double)(voxelGridExtent/voxelSize),3.0);
    // return ((int)pos[0]*prime1^(int)pos[1]*prime2^(int)pos[2]*prime3) % n;
}