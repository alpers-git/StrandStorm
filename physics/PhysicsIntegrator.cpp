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
    for (int i = 0; i < numSteps; i++)
        TakeStep(dt);
    // Call Event Handler to scynronize the rendering geometry with the physics
    for (size_t i = 0; i < scene->rods.size(); i++)
    {
        scene->hairMesh.updateFrom(scene->rods[i], i);
    }
    Event e;
    e.type = Event::Type::PhysicsSync;
    EventHandler::GetInstance().QueueEvent(e);
}

void PhysicsIntegrator::TakeStep(float dt)
{
    // Integrate the physics here
    //  printf("Integrating physics with dt = %f numSteps = %d dt*numSteps = %f\r", dt, numSteps, dt*numSteps);

    // Integrate the elastic rod
    std::for_each(std::execution::par_unseq, scene->rods.begin(), scene->rods.end(), [&](ElasticRod &rod)
                  { rod.integrateFwEuler(dt);});

    std::for_each(std::execution::par_unseq, scene->rods.begin(), scene->rods.end(), [&](ElasticRod &rod)
    {
        rod.enforceConstraints(dt, scene->sceneObjects);
    });
}