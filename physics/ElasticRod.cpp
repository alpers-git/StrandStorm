#include <ElasticRod.hpp>

// Elastic rod sim constants
float ElasticRod::drag = 75.0f;
float ElasticRod::inextensibility = 0.1f;
float ElasticRod::alpha = 0.1f;
Vector3f ElasticRod::gravity = {0.0f, -9.8f, 0.0f};

float ElasticRod::kappa(size_t i)
{
    assert(i > 0);
    float phi = pi - std::acos(edge(i-1).dot(edge(i)) / (edge(i-1).norm() * edge(i).norm()));
    return 2.0f * std::tan(phi / 2.0f);
}

Vector3f ElasticRod::kappaB(size_t i)
{
    return (2.0f * edge(i-1).cross(edge(i))) /
           (initEdge(i).norm() * initEdge(i-1).norm() + edge(i-1).dot(edge(i)));
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
    assert(i >= 0);
    i = std::min(i, xRest.size() - 2);
    return (xRest[i + 1] - xRest[i]).norm();
}

Vector3f ElasticRod::psiGrad(size_t i, size_t j)
{
    assert(j >= i-1 && j <= i+1);
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
    assert(j >= i-1 && j <= i+1);
    const Vector3f kb = kappaB(i);
    const float denom = initEdge(i-1).norm() * initEdge(i).norm() + edge(i-1).dot(edge(i));
    if (j == i - 1) {
        return (2.0f * skew(edge(i)) + kb * edge(i).transpose()) / denom;
    }
    else if (j == i + 1) {
        return (2.0f * skew(edge(i-1)) - kb * edge(i-1).transpose()) / denom;
    }
    else if (j == i) {
        return -((2.0f * skew(edge(i)) + kb * edge(i).transpose()) +
                (2.0f * skew(edge(i-1)) - kb * edge(i-1).transpose())) /
               denom;
    }
}

Vector3f ElasticRod::edge(size_t i)
{
    // If x_i is last vertex, return e_x-1
    assert(i >= 0);
    i = std::min(i, x.size() - 2);
    return x[i + 1] - x[i];
}

Vector3f ElasticRod::initEdge(size_t i)
{
    assert(i >= 0);
    i = std::min(i, xRest.size() - 2);
    return xRest[i + 1] - xRest[i];
}

Vector3f ElasticRod::force(size_t i)
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

    xUnconstrained.resize(verts.size());
    correctionVecs.resize(verts.size());

    for (size_t i = 0; i < verts.size(); i++) 
    {
        x[i] = Vector3f(verts[i][0], verts[i][1], verts[i][2]);
        v[i] = Vector3f::Zero();
        xRest[i] = x[i];

        xUnconstrained[i] = Vector3f::Zero();
        correctionVecs[i] = Vector3f::Zero();        
    }
}


void ElasticRod::integrateFwEuler(float dt)
{
    Eigen::Vector3f vUnconstrained = Vector3f::Zero();
    xUnconstrained[0] = xRest[0]; // so root position is known
    for (size_t i = 1; i < x.size(); i++) 
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
    for (size_t i = 1; i < x.size(); i++) 
    {   
        for (const std::shared_ptr<SceneObject>& c : colliders)
        {
            vertCollider.center = xUnconstrained[i];
            if(c->collider->IsCollidingWith(vertCollider, collisionInfo))     
                xUnconstrained[i] = c->collider->center - 1.01 * collisionInfo.normal * c->collider->GetBoundaryAt(xUnconstrained[i]);
        }      
    }
}

void ElasticRod::enforceConstraints(float dt,const std::vector<std::shared_ptr<SceneObject>>& colliders)
{
    handleCollisions(colliders);

    // Reset positions such that length is maintained and update velocities
    for (size_t i = 1; i < x.size(); i++) 
    {    
        Eigen::Vector3f correctedX = xUnconstrained[i] - xUnconstrained[i-1];
        correctedX = xUnconstrained[i-1] + correctedX.normalized() * initEdgeLen(i-1); 
        v[i] = (correctedX - x[i]) / dt;
        x[i] = correctedX;
        correctionVecs[i] = correctedX - xUnconstrained[i];
        // xUnconstrained[i] = correctedX; // Does this make sense?
    }

    voxelMasses.clear();
    voxelVelocities.clear();

    for (size_t i = 1; i < x.size()-1; i++)
    {
        // Update velocities so that mass distribution is maintained [Refer FTL paper by Muller et al.]
        v[i] -= -inextensibility * correctionVecs[i+1]/dt;

        //Set grid velocities
        setVoxelContributions(x[i], v[i]);
    }

    for (size_t i = 1; i < x.size()-1; i++)
    {
        v[i] = (1-friction) * v[i] + friction * getVoxelVelocity(x[i]);
    }
   
}

void ElasticRod::getVoxelCoordinates(const Eigen::Vector3f& position,Eigen::Vector3f& firstVoxelCoord,Eigen::Vector3f& localPosition)
{
    // Use side length of cube that contains the voxel grid to determine the origin
    Eigen::Vector3f origin = {-voxelGridExtent*0.5f, -voxelGridExtent*0.5f, -voxelGridExtent*0.5f};
    Eigen::Vector3f coordsInVoxel = (position-origin)/voxelSize;
    // Get indices of voxel containing sampling point
    Eigen::Vector3f indices = Eigen::Vector3f(coordsInVoxel.array().floor());
    firstVoxelCoord = indices;
    localPosition = coordsInVoxel - indices;
}

void ElasticRod::setVoxelContributions(const Eigen::Vector3f& position, const Eigen::Vector3f& velocity)
{    
    Eigen::Vector3f firstVoxelCoord,localPosition;
    getVoxelCoordinates(position,firstVoxelCoord,localPosition);
    
    for(size_t i=0;i<=1;i++)
        for(size_t j=0;j<=1;j++)
            for(size_t k=0;k<=1;k++)
            {
                Eigen::Vector3f corner = firstVoxelCoord + Eigen::Vector3f(i,j,k);
                size_t hash = getSpatialHash(corner);
                // corner += boundMin;
                corner -= localPosition;
                corner = Eigen::Vector3f(1.0f,1.0f,1.0f) - Eigen::Vector3f(corner.array().abs());
                if(voxelMasses.find(hash) != voxelMasses.end())
                {
                    voxelMasses[hash] += corner.prod();
                    voxelVelocities[hash] += corner.prod() * velocity;
                }
                else
                {
                    voxelMasses[hash] = corner.prod();
                    voxelVelocities[hash] = corner.prod() * velocity;
                }
            }
}

Eigen::Vector3f ElasticRod::getVoxelVelocity(const Eigen::Vector3f& position)
{
    Eigen::Vector3f firstVoxelCoord,localPosition,velocity;
    getVoxelCoordinates(position,firstVoxelCoord,localPosition);
    Eigen::Vector3f zeroVec = Eigen::Vector3f::Zero();

    // Get the 8 velocities of the 8 corners of the voxel containing the sampling point
    Eigen::Vector3f corner000, corner001,corner100, corner101, corner010, corner011, corner110, corner111;
    sampleVoxelVelocity(corner000,firstVoxelCoord);
    sampleVoxelVelocity(corner001,firstVoxelCoord+Eigen::Vector3f(0,0,1));
    sampleVoxelVelocity(corner100,firstVoxelCoord+Eigen::Vector3f(1,0,0));
    sampleVoxelVelocity(corner101,firstVoxelCoord+Eigen::Vector3f(1,0,1));
    sampleVoxelVelocity(corner010,firstVoxelCoord+Eigen::Vector3f(0,1,0));
    sampleVoxelVelocity(corner011,firstVoxelCoord+Eigen::Vector3f(0,1,1));
    sampleVoxelVelocity(corner110,firstVoxelCoord+Eigen::Vector3f(1,1,0));
    sampleVoxelVelocity(corner111,firstVoxelCoord+Eigen::Vector3f(1,1,1));

    // Perform trilinear interpolation to get the velocity at the sampling point
    Eigen::Vector3f lp_interp1 = (1.0f-localPosition[2])*corner000 + localPosition[2]*corner001;
    Eigen::Vector3f lp_interp2 = (1.0f-localPosition[2])*corner100 + localPosition[2]*corner101;
    Eigen::Vector3f lp = (1.0f-localPosition[0])*lp_interp1 + localPosition[0]*lp_interp2;

    Eigen::Vector3f up_interp1 = (1.0f-localPosition[2])*corner010 + localPosition[2]*corner011;
    Eigen::Vector3f up_interp2 = (1.0f-localPosition[2])*corner110 + localPosition[2]*corner111;
    Eigen::Vector3f up = (1.0f-localPosition[0])*up_interp1 + localPosition[0]*up_interp2;

    velocity = (1.0f-localPosition[1])*lp + localPosition[1]*up;

    return velocity;
}

Eigen::Vector3f ElasticRod::sampleVoxelVelocity(Eigen::Vector3f& vertexVel,const Eigen::Vector3f& index)
{
    size_t hash = getSpatialHash(index);
    if(voxelMasses.find(hash) != voxelMasses.end())
        return voxelVelocities[hash]/voxelMasses[hash];
    else
        return Eigen::Vector3f::Zero();
}

// Based on the paper "Real-time 3D Reconstruction at Scale using Voxel Hashing"
size_t ElasticRod::getSpatialHash(Eigen::Vector3f pos)
{
    size_t n = (size_t)pow((double)(voxelGridExtent/voxelSize),3.0);
    return ((int)pos[0]*prime1^(int)pos[1]*prime2^(int)pos[2]*prime3) % n;
}

void ElasticRod::reset()
{
    for (size_t i = 0; i < x.size(); i++)
    {
        x[i] = xRest[i];
        v[i] = Vector3f::Zero();
    }
}