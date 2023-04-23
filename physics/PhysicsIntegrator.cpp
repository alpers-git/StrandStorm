#include <PhysicsIntegrator.hpp>
#include <sstream>

void PhysicsIntegrator::Initialize()
{
    //Initialize the physics integrator here
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
    Event e;
    e.type = Event::Type::PhysicsSync;
    EventHandler::GetInstance().QueueEvent(e);
}

void PhysicsIntegrator::TakeStep(float dt)
{
    for (int i = 0; i < scene->rods.size(); i++)  {
        scene->rods[i].integrateFwEuler(dt);
        scene->rods[i].enforceConstraints(dt, scene->sceneObjects);
    }
}