#include <ElasticRod.hpp>
#include <Logging.hpp>
#include <spdlog/fmt/fmt.h>

// Elastic rod sim constants
float ElasticRod::drag = 0.0f;
float ElasticRod::inextensibility = 1.0f;
float ElasticRod::alpha = 0.1f;
float ElasticRod::friction = 0.0f;
float ElasticRod::bendingStiffness = 0.1f;
Vector3f ElasticRod::gravity = {0.0f, -0.1f, 0.0f};

Vector3f ElasticRod::kappaB(int i)
{
    return (2.0f * edge(i-1).cross(edge(i))) /
           (initEdge(i).norm() * initEdge(i-1).norm() + edge(i-1).dot(edge(i)));
}

float ElasticRod::initEdgeLen(int i)
{
    return edge(i-1).norm() + edge(i).norm();
}

Vector3f ElasticRod::psiGrad(int i, int j)
{
    assert(j >= i-1 && j <= i+1);
    const Vector3f kb = kappaB(i);
    if (j == i - 1)
    {
        return kb / (2.0f * edge(i - 1).norm());
    }
    else if (j == i + 1)
    {
        return -kb / (2.0f * edge(i).norm());
    }
    return -kb / (2.0f * edge(i - 1).norm()) - kb / (2.0f * edge(i).norm());
}

Matrix3f ElasticRod::kappaBGrad(int i, int j)
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
    return -((2.0f * skew(edge(i)) + kb * edge(i).transpose()) +
        (2.0f * skew(edge(i-1)) - kb * edge(i-1).transpose())) /
        denom;
}

Vector2f ElasticRod::omega(int i, int j) {
    assert(j == i-1 || j == i);
    j = std::clamp(j, 0, (int)(x.size() - 1));
    const Vector3f kb = kappaB(i);
    return {kb.dot(M[j].m2), -kb.dot(M[j].m1)};
}

const Matrix2f J = Rotation2Df(pi / 2.0f).matrix();

Matrix<float, 2, 3> ElasticRod::omegaGrad(int i, int j, int k)
{
    Matrix<float, 2, 3> m;
    m.setZero();
    if (k >= i-1 && k <= i+1) {
        m.row(0) = M[j].m2;
        m.row(1) = -M[j].m1;
        m *= kappaBGrad(i, k);
    }
    return m - J * omega(k, j) * gradHolonomy(i, j).transpose();
}

Vector3f ElasticRod::gradHolonomy(int i, int j)
{
    Vector3f gh = Vector3f::Zero();
    if (j >= i-1 && i > 1 && i-1 < x.size()) {
        gh += gradHolonomyTerms[i-1][2];
    }
    if (j >= i && i < x.size()) {
        gh += gradHolonomyTerms[i][1];
    }
    if (j >= i+1 && i+1 < x.size()) {
        gh += gradHolonomyTerms[i+1][0];
    }
    return gh;
}

Vector3f ElasticRod::dEdX(int i)
{
    const Matrix2f B = Matrix2f::Identity() * this->bendingStiffness;
    Vector3f f = Vector3f::Zero();
    for (int k = 1; k < x.size(); k++) {
        Vector3f pf = Vector3f::Zero();
        for (int j = k-1; j <= k; j++) {
            pf += omegaGrad(i, j, k).transpose() * B * (omega(k, j) - omega0[k][j-k]);
        }
        f += pf / initEdgeLen(k);
    }
    return -f;
}

void ElasticRod::compBishopFrames()
{
    bishopFrames[0] = {u0, edge(0).normalized().cross(u0).normalized()};
    for (int i = 1; i < x.size(); i++) {
        const float cosTheta = edge(i).dot(edge(i-1)) / (edge(i).norm() * edge(i-1).norm());
        if (std::abs(cosTheta) < 1e-6 || cosTheta > 1.0f - 1e-6) {
            bishopFrames[i] = bishopFrames[i-1];
        } else {
            float angle = std::acos(cosTheta);
            assert(!std::isnan(angle));
            AngleAxisf P_i(angle, kappaB(i));
            Vector3f u = (P_i * bishopFrames[i-1].u).normalized();
            Vector3f v = edge(i).normalized().cross(u).normalized();
            bishopFrames[i] = {u, v};
        }
    }
}

void ElasticRod::compMatFrames()
{
    for (int i = 0; i < x.size(); i++) {
        // Since theta is always zero in our case, material frame is same as bishop frame
        /* M[i] = {
            std::cos(theta[i]) * bishopFrames[i].u + std::sin(theta[i]) * bishopFrames[i].v,
            -std::sin(theta[i]) * bishopFrames[i].u + std::cos(theta[i]) * bishopFrames[i].v
        }; */
        M[i] = {
            bishopFrames[i].u,
            bishopFrames[i].v
        };
    }
}

void ElasticRod::compGradHolonomyTerms()
{
    for (int i = 0; i < x.size(); i++) {
        for (int j = 0; j < 3; j++) {
            gradHolonomyTerms[i][j] = psiGrad(i, i + j-1);
        }
    }
}

Vector3f ElasticRod::edge(int i)
{
    i = std::clamp(i, 0, (int)(x.size() - 2));
    return x[i + 1] - x[i];
}

Vector3f ElasticRod::initEdge(int i)
{
    i = std::clamp(i, 0, (int)(x.size() - 2));
    return xRest[i + 1] - xRest[i];
}

Vector3f ElasticRod::force(int i)
{
    assert(i >= 1);
    return dEdX(i);
}

void ElasticRod::init(const std::vector<glm::vec3> &verts)
{
    x.resize(verts.size(), Vector3f::Zero());
    v.resize(verts.size(), Vector3f::Zero());
    gradHolonomyTerms.resize(verts.size());
    theta.resize(verts.size(), 0.0f);
    xUnconstrained.resize(verts.size(), Vector3f::Zero());
    correctionVecs.resize(verts.size(), Vector3f::Zero());
    omega0.resize(verts.size());
    bishopFrames.resize(verts.size());
    M.resize(verts.size());

    for (int i = 0; i < verts.size(); i++)  {
        x[i] = Vector3f(verts[i].x, verts[i].y, verts[i].z);
    }
    xRest = x;
    u0 = edge(0).cross(Vector3f::UnitX()).cross(edge(0)).normalized();

    compBishopFrames();
    compMatFrames();
    compGradHolonomyTerms();

    // Compute initial material curvature
    for (int i = 0; i < verts.size(); i++) {
        for (int j = 0; j < 2; j++) {
            omega0[i][j] = omega(i, i + j-1);
        }
    }

    // Debug printing initial state
    std::stringstream ss;
    ss << "initial state:";
    ss << fmt::format("\nu0 = ({:.3f},{:.3f},{:.3f})", u0.x(), u0.y(), u0.z());
    ss << "\nomega0:";
    for (const auto& omega : omega0) {
        ss << "\n\t";
        for (const Vector2f& v : omega) {
            ss << fmt::format("<{:.3f},{:.3f}>, ", v.x(), v.y());
        }
    }
    ss << "\nM:";
    for (const MaterialFrame& m : M) {
        ss << fmt::format("\n\t<m1=({:.3f},{:.3f},{:.3f}), m2=({:.3f},{:.3f},{:.3f})>",
            m.m1.x(), m.m1.y(), m.m1.z(),
            m.m2.x(), m.m2.y(), m.m2.z());
    }
    
    spdlog::debug(ss.str());
}

ElasticRod::ElasticRod(const std::vector<glm::vec3> &verts)
{
    init(verts);
}


void ElasticRod::integrateFwEuler(float dt)
{
    compBishopFrames();
    compMatFrames();
    compGradHolonomyTerms();

    for (int i = 1; i < x.size(); i++) 
    {
        v[i] += (force(i) + gravity) * dt;
        v[i] -= 0.5f * drag * v[i].squaredNorm() * v[i].normalized() * dt;
        x[i] += v[i] * dt;
    }
}

void ElasticRod::handleCollisions(const std::vector<std::shared_ptr<SceneObject>>& colliders)
{
    SphereCollider vertCollider(Eigen::Vector3f(0.0f, 0.0f, 0.0f), 0.0001f);
    CollisionInfo col;
    for (int i = 1; i < x.size(); i++) {
        for (const std::shared_ptr<SceneObject>& c : colliders) {
            vertCollider.center = x[i];
            if (c->collider->IsCollidingWith(vertCollider, col)) {
                x[i] = c->collider->center - col.normal * c->collider->GetBoundaryAt(x[i]);
            }
        }
    }
}

void ElasticRod::enforceConstraints(float dt,const std::vector<std::shared_ptr<SceneObject>>& colliders)
{
    handleCollisions(colliders);
    x[0] = xRest[0];
    for (int i = 1; i < x.size(); i++) {
        x[i] = x[i-1] + edge(i-1).normalized() * lerp(edge(i-1).norm(), initEdge(i-1).norm(), inextensibility);
    }
}


void ElasticRod::setVoxelContributions(const std::shared_ptr<VoxelGrid>& voxelGrid)
{    
    Eigen::Vector3f firstVoxelCoord,localPosition;
    for (size_t i = 1; i < x.size(); i++) {
        voxelGrid->getVoxelCoordinates(x[i],firstVoxelCoord,localPosition);
        
        for(size_t i=0;i<=1;i++)
        for(size_t j=0;j<=1;j++)
        for(size_t k=0;k<=1;k++) {
            Eigen::Vector3f corner = firstVoxelCoord + Eigen::Vector3f(i,j,k);
            size_t hash = voxelGrid->getSpatialHash(corner);
            corner -= localPosition;
            corner = Eigen::Vector3f(1.0f,1.0f,1.0f) - Eigen::Vector3f(corner.array().abs()); 
            
            voxelGrid->voxelMutex.lock();
            voxelGrid->voxelMasses[hash] += corner.prod();
            voxelGrid->voxelVelocities[hash] += corner.prod() * v[i];
            voxelGrid->voxelMutex.unlock();
        }
    }
}


void ElasticRod::updateAllVelocitiesFromVoxels(const std::shared_ptr<VoxelGrid>& voxelGrid)
{
    Eigen::Vector3f firstVoxelCoord, localPosition, velocity;
    Eigen::Vector3f zeroVec;
    zeroVec.setZero();
    for (size_t i = 1; i < x.size(); i++)
    {
        voxelGrid->getVoxelCoordinates(x[i], firstVoxelCoord, localPosition);

        // Get the 8 velocities of the 8 corners of the voxel containing the sampling point
        Eigen::Vector3f corner000, corner001, corner100, corner101, corner010, corner011, corner110, corner111;
        voxelGrid->sampleVoxelVelocity(corner000, firstVoxelCoord);
        voxelGrid->sampleVoxelVelocity(corner001, firstVoxelCoord + Eigen::Vector3f(0, 0, 1));
        voxelGrid->sampleVoxelVelocity(corner100, firstVoxelCoord + Eigen::Vector3f(1, 0, 0));
        voxelGrid->sampleVoxelVelocity(corner101, firstVoxelCoord + Eigen::Vector3f(1, 0, 1));
        voxelGrid->sampleVoxelVelocity(corner010, firstVoxelCoord + Eigen::Vector3f(0, 1, 0));
        voxelGrid->sampleVoxelVelocity(corner011, firstVoxelCoord + Eigen::Vector3f(0, 1, 1));
        voxelGrid->sampleVoxelVelocity(corner110, firstVoxelCoord + Eigen::Vector3f(1, 1, 0));
        voxelGrid->sampleVoxelVelocity(corner111, firstVoxelCoord + Eigen::Vector3f(1, 1, 1));

        // Perform trilinear interpolation to get the velocity at the sampling point
        Eigen::Vector3f lp_interp1 = (1.0f - localPosition[2]) * corner000 + localPosition[2] * corner001;
        Eigen::Vector3f lp_interp2 = (1.0f - localPosition[2]) * corner100 + localPosition[2] * corner101;
        Eigen::Vector3f lp = (1.0f - localPosition[0]) * lp_interp1 + localPosition[0] * lp_interp2;

        Eigen::Vector3f up_interp1 = (1.0f - localPosition[2]) * corner010 + localPosition[2] * corner011;
        Eigen::Vector3f up_interp2 = (1.0f - localPosition[2]) * corner110 + localPosition[2] * corner111;
        Eigen::Vector3f up = (1.0f - localPosition[0]) * up_interp1 + localPosition[0] * up_interp2;

        velocity = (1.0f - localPosition[1]) * lp + localPosition[1] * up;
        assert(!velocity.hasNaN());
        v[i] = lerp(v[i], velocity, friction);
    }
}

void ElasticRod::reset()
{
    for (int i = 0; i < x.size(); i++) {
        x[i] = xRest[i];
        v[i] = Vector3f::Zero();
    }
}
