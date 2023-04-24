#include <PhysicsIntegrator.hpp>
#include <algorithm>
#include <execution>
#include <sstream>


void PhysicsIntegrator::Initialize()
{
    // Initialize the physics integrator here
}

void PhysicsIntegrator::Integrate()
{
    for (int i = 0; i < numSteps; i++) {
        TakeStep(dt);
    }
    //Call Event Handler to scynronize the rendering geometry with the physics
    for (size_t i = 0; i < scene->rods.size(); i++) {
        scene->hairMesh.updateFrom(scene->rods[i], i);
    }
    // Call Event Handler to scynronize the rendering geometry with the physics
    Event e;
    e.type = Event::Type::PhysicsSync;
    EventHandler::GetInstance().QueueEvent(e);
}

void PhysicsIntegrator::TakeStep(float dt)
{
    scene->voxelGrid->initVoxelGrid();
    // Integrate the physics here
    std::for_each(std::execution::par_unseq, scene->rods.begin(), scene->rods.end(), [&](ElasticRod &rod)
    { 
        rod.integrateFwEuler(dt);
    });

    std::for_each(std::execution::par_unseq, scene->rods.begin(), scene->rods.end(), [&](ElasticRod &rod)
    {
        rod.enforceConstraints(dt, scene->sceneObjects);
    });

    if (ElasticRod::friction > 0.0f) {
        std::for_each(std::execution::par_unseq, scene->rods.begin(), scene->rods.end(), [&](ElasticRod &rod)
        {
            rod.setVoxelContributions(scene->voxelGrid);
        });

        std::for_each(std::execution::par_unseq, scene->rods.begin(), scene->rods.end(), [&](ElasticRod &rod)
        {
            rod.updateAllVelocitiesFromVoxels(scene->voxelGrid);
        });
    }
}
