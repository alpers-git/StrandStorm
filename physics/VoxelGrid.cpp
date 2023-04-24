#include <VoxelGrid.hpp>
#include <Util.hpp>

VoxelGrid::VoxelGrid()
{
    voxelMasses.resize(nVoxels(), 0.0f);
    voxelVelocities.resize(nVoxels(), Eigen::Vector3f(0.0f, 0.0f, 0.0f));
}

void VoxelGrid::initVoxelGrid()
{
    voxelMasses.assign(nVoxels(), 0.0f);
    voxelVelocities.assign(nVoxels(), Eigen::Vector3f(0.0f, 0.0f, 0.0f));
}

void VoxelGrid::getVoxelCoordinates(const Eigen::Vector3f &position, Eigen::Vector3f &firstVoxelCoord, Eigen::Vector3f &localPosition)
{
    // Use side length of cube that contains the voxel grid to determine the origin
    Eigen::Vector3f extent(Eigen::Vector3f::Constant(voxelGridExtent));
    Eigen::Vector3f corner(origin - (extent / 2.0f));
    Eigen::Vector3f coordsInVoxel = (position-corner)/voxelSize;
    // Get indices of voxel containing sampling point
    Eigen::Vector3f indices = Eigen::Vector3f(coordsInVoxel.array().floor())
        .cwiseMin(extent).cwiseMax(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
    firstVoxelCoord = indices;
    localPosition = coordsInVoxel - indices;
    assert(!localPosition.hasNaN());
    assert(!firstVoxelCoord.hasNaN());
}

void VoxelGrid::sampleVoxelVelocity(Eigen::Vector3f &vertexVel, const Eigen::Vector3f &index)
{
    size_t hash = getSpatialHash(index);
    assert(hash >= 0 && hash < nVoxels());
    float norm = voxelMasses[hash] == 0 ? 1 : voxelMasses[hash];
    assert(!(voxelVelocities[hash]/norm).hasNaN());
    vertexVel = voxelVelocities[hash]/norm;
}

// Based on the paper "Real-time 3D Reconstruction at Scale using Voxel Hashing"
size_t VoxelGrid::getSpatialHash(const Eigen::Vector3f& pos)
{
    return (size_t)std::floor(pos[0])*cellExtent()*cellExtent()
        + (size_t)std::floor(pos[1])*cellExtent()
        + (size_t)std::floor(pos[2]);
}